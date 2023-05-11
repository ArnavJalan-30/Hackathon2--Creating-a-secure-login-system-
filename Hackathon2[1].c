#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <time.h>

#define HASH_SIZE 20 // Size of the hash in bytes
#define SALT_SIZE 8 // Size of the salt in bytes
#define TABLE_SIZE 10 // Size of the hash table

// Simple hash function that adds up the numbers for each character in a string
int
hash(char * str, char * salt) {
        int sum = 0;
        for (int i = 0; i < strlen(str); i++) {
                sum += (int) str[i];
        }
        for (int i = 0; i < strlen(salt); i++) {
                sum += (int) salt[i];
        }
        return sum;
}

// Generate a random salt string
void
generate_salt(char * salt, int size,
        const char * username,
                const char * password) {
        const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // The characters to use in the salt
        unsigned int seed = 0;
        for (int i = 0; username[i] != '\0'; i++) {
                seed += (unsigned int) username[i];
        }
        for (int i = 0; password[i] != '\0'; i++) {
                seed += (unsigned int) password[i];
        }
        srand(seed); // Set the seed for the random number generator
        for (int i = 0; i < size; i++) {
                salt[i] = charset[rand() % strlen(charset)]; // Pick a random character from the charset and add it to the salt
        }
        salt[size] = '\0'; // End the salt string

}

// Define a node for the hash table linked list
struct node {
        char * username;
        int hash_val;
        struct node * next;
};

// Function to print a hash table
void
print_hash_table(struct node ** table, int size, int count) {
        printf("Number of registered users: %d\n\n", count);
        printf("Hash table:\n");
        for (int i = 0; i < size; i++) {
                printf("%d: ", i);
                struct node * current = table[i];
                while (current != NULL) {
                        printf("\t====\t%08x ", current -> hash_val);
                        current = current -> next;
                }
                printf("\n");
        }
}

void
main() {
        // Initialize the hash table as an array of NULL pointers
        struct node * hash_table[TABLE_SIZE] = {
                NULL
        };

        int choice;
        char username[100];
        char password[100];
        char confirm_password[100];
        char salt[SALT_SIZE + 1];
        int hash_val;
        int flag = 1, count = 0;

        while (flag == 1) {
                printf("\nWelcome:\n");
                printf("1. Register and create a new account\n");
                printf("2. Log In\n");
                printf("3. Delete account\n");
                printf("4. Print hash table\n");
                printf("0. Exit\n");
                printf("Enter choice: ");
                scanf("%d", & choice);

                switch (choice) {
                case 1:
                        // Put in new username and password
                        printf("Enter new username: ");
                        scanf("%s", username);
                        printf("Enter new password: ");
                        scanf("%s", password);

                        // Make a random salt
                        generate_salt(salt, SALT_SIZE, username, password);

                        // Mix the password and salt together and hash the result
                        char combined[100 + SALT_SIZE + 1]; // Allocate space for the mixed string
                        strcpy(combined, password); // Copy the password into the mixed string
                        strcat(combined, salt); // Add the salt onto the end of the mixed string
                        hash_val = hash(combined, salt); // Hash the mixed string with the salt

                        // Create a new node for the hash value
                        struct node * new_node = malloc(sizeof(struct node));
                        new_node -> username = username;
                        new_node -> hash_val = hash_val;
                        new_node -> next = NULL;

                        // Add the new node to the hash table
                        int index = hash_val % TABLE_SIZE; // Use the hash value to find an index in the hash table
                        if (hash_table[index] == NULL) {
                                hash_table[index] = new_node;
                        } else {
                                struct node * current = hash_table[index];
                                while (current -> next != NULL) {
                                        current = current -> next;
                                        current -> next = new_node;
                                }
                        }
                        printf("New account added successfully!\n");
                        count++;
                        break;

                case 2:
                        // Prompt the user for a username and password
                        printf("Enter your username: ");
                        scanf("%s", username);
                        printf("Enter your password: ");
                        scanf("%s", password);

                        generate_salt(salt, SALT_SIZE, username, password);
                        strcpy(combined, password); // Copy the password into the mixed string
                        strcat(combined, salt); // Add the salt onto the end of the mixed string
                        hash_val = hash(combined, salt); // Hash the mixed string with the salt
                        // Use the username to find the hash value in the hash table
                        index = hash_val % TABLE_SIZE; // Use the hash function to find the index
                        struct node * current = hash_table[index];
                        while (current != NULL) {
                                if (current -> username != NULL &&
                                        strcmp(current -> username, username) == 0) { // Check if the username matches

                                        int hash_val = hash(combined, salt); // Hash the mixed string with the salt
                                        if (hash_val == current -> hash_val) { // Check if the hash values match
                                                printf("Login successful.\n");
                                        } else {
                                                printf("Incorrect password.\n");
                                        }
                                        break; // Exit the loop once the matching node has been found
                                }
                                current = current -> next;
                        }
                        if (current == NULL) { // If the loop has finished without finding a matching node
                                printf("Username not found for this password.\n");
                        }
                        break;

                case 3:
                        // Ask for the username to delete
                        printf("Enter the username to delete: ");
                        scanf("%s", username);
                        printf("Enter your password: ");
                        scanf("%s", password);

                        generate_salt(salt, SALT_SIZE, username, password);
                        strcpy(combined, password); // Copy the password into the mixed string
                        strcat(combined, salt); // Add the salt onto the end of the mixed string
                        hash_val = hash(combined, salt); // Hash the mixed string with the salt
                        // Use the hash function to get the index in the hash table for this username
                        index = hash_val % TABLE_SIZE; // Use the hash function to find the index

                        // Find the node for this username and delete it
                        current = hash_table[index];
                        struct node * prev = NULL;
                        while (current != NULL) {
                                // If we find the node for this username, delete it
                                if (current -> hash_val == hash_val) {
                                        if (prev == NULL) {
                                                // This node is the first in the linked list
                                                hash_table[index] = current -> next;
                                        } else {
                                                // This node is not the first in the linked list
                                                prev -> next = current -> next;
                                        }
                                        free(current);
                                        printf("Deleted username and password for %s\n", username);
                                        count--;
                                        break;
                                }
                                // Move to the next node in the linked list
                                prev = current;
                                current = current -> next;
                        }

                        // If we didn't find the username in the hash table, print an error message
                        if (current == NULL) {
                                printf("Error: username not found\n");
                        }

                        break;

                case 4:
                        print_hash_table(hash_table, TABLE_SIZE, count);
                        break;

                case 0:
                        flag = 0;
                        break;
                }
        }
}