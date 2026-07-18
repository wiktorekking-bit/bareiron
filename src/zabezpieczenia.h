#ifndef ZABEZPIECZENIA_H
#define ZABEZPIECZENIA_H

#include <string.h>
#include <stdio.h>

// Definiujemy ścieżki do plików w pamięci stałej ESP32 (LittleFS)
#define AUTH_FILE "/littlefs/ip_database.txt"
#define LOG_FILE  "/littlefs/zapis_ip.txt"

#define MAX_PLAYERS 20
#define NAME_LEN 16
#define IP_LEN 16

// Struktura przechowująca parę: Nick i jego zapisane IP
struct PlayerAuth {
    char username[NAME_LEN];
    char secure_ip[IP_LEN];
};

// Nasza baza danych w pamięci RAM mikrokontrolera
struct PlayerAuth server_database[MAX_PLAYERS];
int total_registered = 0;

// 1. FUNKCJA: Wczytuje główną bazę z pliku tekstowego przy starcie serwera
void load_ip_database() {
    FILE *f = fopen(AUTH_FILE, "r");
    if (f == NULL) {
        printf("[Baza IP] Brak pliku bazy. Zostanie utworzony przy pierwszym graczu.\n");
        return;
    }

    total_registered = 0;
    while (fscanf(f, "%s %s", server_database[total_registered].username, server_database[total_registered].secure_ip) == 2) {
        total_registered++;
        if (total_registered >= MAX_PLAYERS) break;
    }
    fclose(f);
    printf("[Baza IP] Wczytano %d graczy z pliku pamięci.\n", total_registered);
}

// 2. FUNKCJA: Zapisuje główną bazę danych do pliku konfiguracyjnego
void save_ip_database() {
    FILE *f = fopen(AUTH_FILE, "w");
    if (f == NULL) {
        printf("[BŁĄD] Nie można zapisać bazy IP do pamięci flash!\n");
        return;
    }

    for (int i = 0; i < total_registered; i++) {
        fprintf(f, "%s %s\n", server_database[i].username, server_database[i].secure_ip);
    }
    fclose(f);
    printf("[Baza IP] Zapisano główną bazę danych do pliku.\n");
}

// 3. FUNKCJA DODATKOWA: Zapisuje czytelną informację do pliku "zapis_ip.txt"
void zapisz_do_kroniki_ip(const char* nazwa_gracza, const char* zarejestrowane_ip) {
    FILE *f_log = fopen(LOG_FILE, "a");
    if (f_log == NULL) {
        printf("[BŁĄD] Nie można otworzyć pliku zapis_ip.txt!\n");
        return;
    }
    fprintf(f_log, "Gracz: %s | IP: %s\n", nazwa_gracza, zarejestrowane_ip);
    fclose(f_log);
    printf("[Kronika] Dodano nowy wpis do pliku: zapis_ip.txt\n");
}

// 4. FUNKCJA GŁÓWNA: Bramkarz serwera weryfikujący IP gracza
void authorize_player_connection(int client_socket, const char* incoming_name, const char* incoming_ip) {
    load_ip_database();
    int player_index = -1;

    for (int i = 0; i < total_registered; i++) {
        if (strcmp(server_database[i].username, incoming_name) == 0) {
            player_index = i;
            break;
        }
    }

    // --- REJESTRACJA ---
    if (player_index == -1) {
        if (total_registered < MAX_PLAYERS) {
            strncpy(server_database[total_registered].username, incoming_name, NAME_LEN - 1);
            strncpy(server_database[total_registered].secure_ip, incoming_ip, IP_LEN - 1);
            total_registered++;
            
            save_ip_database(); 
            zapisz_do_kroniki_ip(incoming_name, incoming_ip);
            
            printf("[Baza IP] Zarejestrowano pomyślnie nowy nick: %s\n", incoming_name);
        } else {
            printf("[Baza IP] Brak wolnego miejsca na serwerze!\n");
        }
        return;
    }

    // --- WERYFIKACJA ---
    if (strcmp(server_database[player_index].secure_ip, incoming_ip) != 0) {
        printf("[BLOKADA IP] Wykryto próbę podsunięcia się! %s z IP: %s\n", incoming_name, incoming_ip);
        
        // Funkcja wyrzucająca (nazwę dopasujemy w main.c)
        disconnect_client(client_socket); 
        return;
    }

    printf("[Baza IP] Gracz %s zweryfikowany zaufanym adresem IP.\n", incoming_name);
}

#endif // ZABEZPIECZENIA_H
