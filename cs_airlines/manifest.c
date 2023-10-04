// CS Airline Manifest
// manifest.c
//
// This program was written by Rocky Liu (z5349042)
// on 2022/4/16
//
//
// This is an implemention file for manifest.h
// manifest.c will be used in stage 4:
// 1.ADD_PERSON 
// 2.PRINT_MANIFEST 
// 3.FLIGHT_STATS
// 

#include "manifest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct person {
    char name[MAX_NAME_LEN];
    double weight;
    struct person *next;
};

// Internal Function Prototypes
struct person *create_person(char name[MAX_NAME_LEN], double weight);

struct manifest *create_manifest() {
    struct manifest *manifest = malloc(sizeof(struct manifest));
    manifest->people = NULL;
    return manifest;
}

void print_manifest(struct manifest *manifest) {
    printf("Manifest:\n");

    if (manifest == NULL) {
        return;
    }

    // Loop through list
    struct person *curr = manifest->people;
    while (curr != NULL) {
        printf("    %03.2lf - %s\n", curr->weight, curr->name);
        curr = curr->next;
    }
}

// PROVIDED FUNCTION
// Given person info, mallocs space for the person
// Parameters:
//     name        = name of the new person
//     weight      = weight of the new person
// Returns:
//     pointer to the malloc'd person
struct person *create_person(char name[MAX_NAME_LEN], double weight) {
    // Malloc the new person
    struct person *new_person = malloc(sizeof(struct person));

    // Copy data
    strcpy(new_person->name, name);
    new_person->weight = weight;
    new_person->next = NULL;

    return new_person;
}

void add_person(struct manifest *manifest, char name[MAX_NAME_LEN],
                double weight) {
    // Create a new person
    struct person *new_person = create_person(name, weight);

    // Check if list is empty
    if (manifest->people == NULL) {
        manifest->people = new_person;
        return;
    }

    // Find the end of the list
    struct person *curr = manifest->people;
    while (curr->next != NULL) {
        curr = curr->next;
    }

    // Curr now points to the last person in the list
    // Add the new person to the end of the list
    curr->next = new_person;
}

void delete_manifest(struct manifest *manifest) {

    if (manifest == NULL) {
        return;
    }

    // Free the list of people
    struct person *curr = manifest->people;
    while (curr != NULL) {
        
        struct person *temp = curr->next;
        free(curr);
        curr = temp;
    }

    // Free the malloc
    free(manifest);
}

// Calculate the weight of a manifest
double manifest_weight(struct manifest *manifest) {
    double total_manifest_weight = 0.00;
    // Give a pointer to manifest->people
    struct person *current = manifest->people;
    // add every person's weight
    while (current != NULL) {
        total_manifest_weight += current->weight;
        current = current->next;
    }
    return total_manifest_weight;
}

// Move the people from the manifest_1 and then add them to the manifest_2
struct manifest *join_manifest(struct manifest *manifest_1, struct manifest *manifest_2) {
    if (manifest_1->people != NULL && manifest_2->people == NULL) {
        struct person *temp = manifest_1->people;
        // add manifest_1 people to the manifest_2
        manifest_2->people = temp;
        // remove the people from manifest_1
        manifest_1->people = NULL;
        return manifest_2;
    } else if (manifest_1->people != NULL && manifest_2->people != NULL) {
        struct person *current = manifest_2->people;
        while (current->next != NULL) {
            current = current->next;
        }
        // add manifest_1 people to the manifest_2
        current->next = manifest_1->people;
        // remove the people from manifest_1
        manifest_1->people = NULL;
        return manifest_2;
    } else {
        return manifest_2;
    }
}
