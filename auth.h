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
    int xp;
} UserData;

extern UserData* current_user;

void authentication_window();
void signup_window();
void login_window();
int store_user_data(char* username, char* password, char* email);
int user_exists(char* username);
int save_user_data(UserData* user_data);
int read_user_data(UserData* user_data);
int signup_user(char* username, char* password, char* email);
int login_user(char* username, char* password);
void logout_user(void);
void check_logged_in_user(void);
int password_length_valid(char* password);
int password_final_validation(char* password);
int email_valid(char* email);
int save_current_user(UserData* user_data);
int load_current_user(UserData* user_data);
int save_new_user_list(char * username);
char * random_password();
char * HintPass(char * username);
#endif
