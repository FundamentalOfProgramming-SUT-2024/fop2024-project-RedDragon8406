#ifndef AUTH_H
#define AUTH_H

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define USERS_DIR "users/"

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int status;
    int points;
    int golds;
} UserData;

extern UserData* current_user;

void authentication_window();
void signup_window();
void login_window();
int store_user_data(const char* username, const char* password, const char* email);
int user_exists(const char* username);
int save_user_data(UserData* user_data);
int read_user_data(UserData* user_data);
int signup_user(const char* username, const char* password, const char* email);
int login_user(const char* username, const char* password);
void logout_user(void);
void check_logged_in_user(void);
int password_length_valid(const char* password);
int password_final_validation(const char* password);
int email_valid(const char* email);
int save_current_user(UserData* user_data);
int load_current_user(UserData* user_data);
#endif
