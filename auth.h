#ifndef AUTH_H
#define AUTH_H

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define USERS_DIR "users/"

// Function prototypes
void create_user_file(const char* username, const char* password, const char* email);
int user_exists(const char* username);
int validate_email(const char* email);
void signup_window();
void authentication_window();
int store_user_data(const char* username, const char* password, const char* email);
int signup_user(const char* username, const char* password, const char* email) ;
int user_exists(const char* username);
int password_length_valid(const char* password);
int password_final_validation(const char* password);

#endif
