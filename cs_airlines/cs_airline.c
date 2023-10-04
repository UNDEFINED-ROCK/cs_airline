// CS Airline
// cs_airline.c
//
// This program was written by Rocky Liu (z5349042)
// on 2022/4/16
//
// This program is a booking system capable of managing a single 
// flight with multiple stops.
// This program provides 
// these commands:
// 1.PRINT_HELP 
// STAGE 1:
//      2.PRINT_ROUTE 
// STAGE 2:
//      3.SUBROUTE 4.CHANGE_ORIGIN 5.BYPASS 
// STAGE 3:
//      6.EMERGENCY  7.CANCEL 8.REVERSE  
// STAGE 4:
//      9.ADD_PERSON 10.PRINT_MANIFEST 11.FLIGHT_STATS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manifest.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// The buffer length is used for reading a single line
#define MAX_STRING_LEN 100

#define COMMAND_PRINT_HELP "help"
#define COMMAND_PRINT_ROUTE "print"
#define COMMAND_SUBROUTE "subroute"
#define COMMAND_CHANGE_ORIGIN "change_origin"
#define COMMAND_BYPASS "bypass"
#define COMMAND_EMERGENCY "emergency"
#define COMMAND_CANCEL "cancel"
#define COMMAND_REVERSE "reverse"
#define COMMAND_ADD_PERSON "add_person"
#define COMMAND_PRINT_MANIFEST "print_manifest"
#define COMMAND_FLIGHT_STATS "stats"

// define TRUE and FALSE to make judgemnet
#define TRUE 1
#define FALSE 0

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Airports represent 'stops' along the flight path
// These are the 'nodes' of the linked list
struct airport {
    char code[MAX_STRING_LEN];
    int arrival_time;
    int departure_time;
    struct airport *next_airport;
    struct manifest *manifest;
};

// Root flight structure
// This stores the state of the current flight
// (i.e. the head and tail of the linked list)
struct flight {
    struct airport *origin_airport;
    struct airport *dest_airport;
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Helper Functions
void remove_newline(char input[MAX_STRING_LEN]);
void do_print_help(void);

void interpret_line(
    char buffer[MAX_STRING_LEN],
    int *val0,
    int *val1,
    char word[MAX_STRING_LEN]
);

// Stage 1 Functions
struct flight *setup_flight(void);
void do_print_airports(struct flight *flight);
void print_one_airport(struct airport *ap);
struct airport *create_airport(
    char code[MAX_STRING_LEN],
    int arrival_time,
    int departure
);

// Stage 2 Functions
void do_subroute(struct flight *flight);
void do_change_origin(struct flight *flight);


void append_airport(
    struct flight *flight, 
    char airport_code[MAX_STRING_LEN], 
    int arrival_time, 
    int departure_time
);
void do_bypass(struct flight *flight);
void  do_emergency(struct flight *flight);
void airport_emergency(struct flight *flight, int emergency_time);
void do_cancel(struct flight *flight);
struct flight *do_reverse(struct flight *flight);
void do_add_person(struct flight *flight);
void do_print_manifest(struct flight *flight);
void do_flight_stats(struct flight *flight);
////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    // Stage 1.1 - Initialisation
    struct flight *flight = setup_flight();

    printf("Enter Command: ");
    char command_line[MAX_STRING_LEN];
    while (fgets(command_line, MAX_STRING_LEN, stdin) != NULL) {

        remove_newline(command_line);

        if (strcmp(COMMAND_PRINT_HELP, command_line) == 0) {
            // A help command we have implemented for you.
            do_print_help();
        } else if (strcmp(COMMAND_PRINT_ROUTE, command_line) == 0) {
            // Stage 1.2 - Print Route
            do_print_airports(flight);
        } else if (strcmp(COMMAND_SUBROUTE, command_line) == 0) {
            // Stage 2.1 - Calculate Subroute
            do_subroute(flight);
        } else if (strcmp(COMMAND_CHANGE_ORIGIN, command_line) == 0) {
            // Stage 2.2 - Change Origin
            do_change_origin(flight);
        } else if (strcmp(COMMAND_BYPASS, command_line) == 0) {
            // Stage 2.3 - Bypass Airport
            do_bypass(flight);
        } else if (strcmp(COMMAND_EMERGENCY, command_line) == 0) {
            // Stage 3.1 - Emergency Landing
            do_emergency(flight);
        } else if (strcmp(COMMAND_CANCEL, command_line) == 0) {
            // Stage 3.2 - Cancel Flight
            do_cancel(flight);
            printf("Flight cancelled. Now accepting a new flight: \n");
            flight = setup_flight();
        } else if (strcmp(COMMAND_REVERSE, command_line) == 0) {
            // Stage 3.3 - Reverse Flight
            flight = do_reverse(flight);
        } else if (strcmp(COMMAND_ADD_PERSON, command_line) == 0) {
            // Stage 4.1 - Add People to Airport
            do_add_person(flight);
        } else if (strcmp(COMMAND_PRINT_MANIFEST, command_line) == 0) {
            // Stage 4.2 - Print Airport Manifest
            do_print_manifest(flight);
        } else if (strcmp(COMMAND_FLIGHT_STATS, command_line) == 0) {
            // Stage 4.3 - Flight Statistics
            do_flight_stats(flight);
        }
            
        printf("Enter Command: ");
    }
    // free the lists(people, manifests, airports, flights)
    // after EOF
    do_cancel(flight);
    printf("Goodbye!\n");

    return 0;
}

// Stage 1.1
// Creates and initialises a flight by asking the user for input.
// Parameters:
//     None
// Returns:
//     Pointer to the malloc'd flight
struct flight *setup_flight(void) {

    // Create a new, empty flight
    struct flight *new_flight = malloc(sizeof(struct flight));
    new_flight->origin_airport = NULL;
    new_flight->dest_airport = NULL;

    // Read number of stops using fgets
    printf("How many stops will be enroute? ");
    char input_line[MAX_STRING_LEN] = {0};
    fgets(input_line, MAX_STRING_LEN, stdin);

    // Convert the string to a number
    int num_stops = 0;
    num_stops = atoi(input_line);
    int append_index = 0;
    int previous_departure_time = 0;
    
    // Add some airports to the flight
    while (append_index < num_stops) {

        // Read new airport information using fgets
        char new_airport_info[MAX_STRING_LEN] = {0};
        fgets(new_airport_info, MAX_STRING_LEN, stdin);

        // Use the function interpert_line() to interpret the inputted information
        int arrival_time = 0;
        int departure_time = 0;
        char airport_code[MAX_STRING_LEN];
        interpret_line(
            new_airport_info, 
            &arrival_time, 
            &departure_time, 
            airport_code);
        
        if (arrival_time > departure_time) {
            printf("Departure time cannot be before the arrival time!\n");
        } else if (previous_departure_time > arrival_time) {
            printf("New arrival time cannot be before the previous departure time\n");
        } else {
            // The function append_airport() append the new 
            // airport informantion to the flight.
            append_airport(
                new_flight, 
                airport_code, 
                arrival_time, 
                departure_time);
            // Count the departure time of the new airport as 
            // the previous departure time.
            previous_departure_time = departure_time;
        }
        append_index++;
    }

    return new_flight;
}

// Stage 1.1
// Creates a new airport and appends it to the flight
// Parameters:
//     flight          = the flight which contains the flight route
//     airport_code    = the airport ICAO code
//     arrival_time    = the arrival time of the new airport
//     departure_time  = the departure time of the new airport
// Returns:  
//     None
void append_airport(
    struct flight *flight, 
    char airport_code[MAX_STRING_LEN], 
    int arrival_time, 
    int departure_time) 
{
    // Create a new "node" for a new airport
    struct airport *new_airport = create_airport(
        airport_code, 
        arrival_time, 
        departure_time);
    // Give a pointer to the "head" of the flight(flight->origin_airport)
    struct airport *current = flight->origin_airport;
    if (flight->origin_airport == NULL) {
        // If there is no airport, create the 
        // flight->origin_airport (origin airport)
        flight->origin_airport = new_airport;
    } else {
        // If there is at least one airport, 
        // find the "tail" of the flight and then
        // add a new airport 
        while (current->next_airport != NULL) {
            current = current->next_airport;
        }
        current->next_airport = new_airport;
    } 
    // A new airport will be set as the destination
    // (flight->dest_airport)
    flight->dest_airport = new_airport;
}

// Stage 1.2
// Given a pointer to a flight struct, prints all the info about the route
// Parameters:
//     flight = the flight which contains the flight route to print
// Returns:
//     None
void do_print_airports(struct flight *flight) {
    struct airport *current = flight->origin_airport;
    printf("ROUTE:\n");
    while (current != NULL) {
        print_one_airport(current);
        current = current->next_airport;
    }
}

// Stage 1.2
// PROVIDED FUNCTION - DO NOT CHANGE
// Given a pointer to an airport struct,
// prints all the info about the airport
// Parameters:
//     ap = the airport to print
// Returns:
//     None
void print_one_airport(struct airport *ap) {
    printf(
        "%8s:    %04d -> %04d\n",
       ap->code,
       ap->arrival_time,
       ap->departure_time
    );
}

// Stage 1.1
// Given the information about a new airport,
// Uses `malloc` to create memory for it and returns a pointer to
// that memory.
// Parameters:
//     code            = the airport ICAO code
//     arrival_time    = the arrival time of the new airport
//     departure_time  = the departure time of the new airport
// Returns:
//     A pointer to the malloc'd struct airport
struct airport *create_airport(
    char code[MAX_STRING_LEN],
    int arrival_time,
    int departure_time)
{
    // Malloc new struct
    struct airport *new_airport = malloc(sizeof(struct airport));

    // initialise airport fields
    strcpy(new_airport->code, code);
    new_airport->arrival_time = arrival_time;
    new_airport->departure_time = departure_time;
    new_airport->next_airport = NULL;
    new_airport->manifest = create_manifest();
    return new_airport;
}

// Stage 2.1
// This function implements the subroute command:
// Calculates the number of hours and minutes 
// between the origin airport's departure time, 
// and the given airport's arrival time and
// prints the time difference out
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_subroute(struct flight *flight) {
    // use the variable "existed_code" to 
    // show that the airport ICA0 code exists or not
    // existed_code = FALSE: The code does not exist.
    // existed_code = TRUE: The code exists.
    int existed_code = FALSE;
    int start = 0;
    int end = 0;

    // Give a pointer to the "head" of the flight(flight->origin_airport)
    struct airport *current = flight->origin_airport;

    // Read the airport ICAO code using fgets
    printf("Enter airport code: ");
    char airport_code[MAX_STRING_LEN] = {0};
    fgets(airport_code, MAX_STRING_LEN, stdin);
    remove_newline(airport_code);

    // Count the departure time of 
    // the origin airport(flight->origin_airport->departure_time)
    // as "start"
    start = flight->origin_airport->departure_time;
    while (current != NULL) { 
        if (strcmp(current->code, airport_code) == 0) {
            // The given code exists.
            // Change the value of "existed_code" to "TRUE"
            existed_code = TRUE;
            // Count the arrival time of the current airport
            // as "end"
            end = current->arrival_time;
        }
        current = current->next_airport;
    }
    if (existed_code == TRUE) {
        // Calculate the time difference between "start" and "end"
        int hours = (end / 100) - (start / 100);
        int mins = (end % 100) - (start % 100);
        int minute_diff = hours * 60 + mins;
        if (minute_diff < 0) {
            minute_diff *= -1;
        }
        hours = minute_diff / 60;
        mins = minute_diff % 60;
        printf("Time for subroute: %d hrs & %d mins\n", hours, mins);
    } else {
        printf("Desired subroute destination does not exist!\n");
    }
}

// Stage 2.2
// This function implements the change_origin command:
// Inserts a new airport before the existing origin 
// and then sets this new airport as the new origin airport
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_change_origin(struct flight *flight) {
    // Give a pointer to the "head" of the flight(flight->origin_airport)
    struct airport *current = flight->origin_airport;

    // The variable "unique_code" shows that the code is unique or not.
    // unique_code = FALSE: The code is not unique.
    // unique_code = TRUE: The code is unique.
    int unique_code = TRUE;

    // Read new origin airport information using fgets
    char new_origin_info[MAX_STRING_LEN] = {0};
    printf("Enter new origin info: ");
    fgets(new_origin_info, MAX_STRING_LEN, stdin);

    // Use the function interpert_line() to 
    // interpret the new origin airport information
    int arrival_time = 0;
    int departure_time = 0;
    char new_origin_code[MAX_STRING_LEN];
    interpret_line(
        new_origin_info, 
        &arrival_time, 
        &departure_time, 
        new_origin_code);

    // use while loop to determine that the 
    // given code is unique or not
    while (current != NULL) {
        if (strcmp(current->code, new_origin_code) == 0) {
            unique_code = FALSE;
        }
        current = current->next_airport;
    }

    if (arrival_time > departure_time ) {
        printf("Departure time cannot be before the arrival time!\n");
    } else if (departure_time > flight->origin_airport->arrival_time) {
        printf("Departure of new origin cannot ");
        printf("be after the arrival time of the next airport!\n");
    } else if (unique_code == FALSE) {
        printf("New airport code is not unique!\n");
    } else {
        // Create a new origin airport(new_origin_airport)
        struct airport *new_origin_airport = create_airport(
            new_origin_code, 
            arrival_time, 
            departure_time);
        current = flight->origin_airport;
        new_origin_airport->next_airport = current;
        // Set the new_origin_airport as the origin
        flight->origin_airport = new_origin_airport;
        printf("Added: %s\n", new_origin_code);
    }
}

// Stage 2.3
// This function implement the bypass command:
// Frees the desired people, manifest, airport
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_bypass(struct flight *flight) {
    // use the variable "existed_code" to 
    // show that the airport ICA0 code exists or not
    // existed_code = FALSE: The code does not exist.
    // existed_code = TRUE: The code exists.
    int existed_code = FALSE;
    
    // Give a pointer to the "head" of the flight(flight->origin_airport)
    struct airport *current = flight->origin_airport;

    // use fgets to read the airport code which is desired to be bypassed 
    printf("Enter airport code: ");
    char bypass_code[MAX_STRING_LEN] = {0};
    fgets(bypass_code, MAX_STRING_LEN, stdin);
    remove_newline(bypass_code);

    if (strcmp(flight->origin_airport->code, bypass_code) == 0) {
        existed_code = TRUE;

        // Give a pointer to the airport next to the origin airport
        struct airport *new_origin_airport = current->next_airport;

        // use the function delete_manifest() from manifest.c  
        // to free the manifest and the people
        delete_manifest(current->manifest);

        // Free the airport
        free(current);

        // Set new_origin_airport as the origin(flight->origin_airport)
        flight->origin_airport = new_origin_airport;
    } else {
        // Find the airport which is desired to be bypassed
        while ((strcmp(current->next_airport->code, bypass_code) != 0) && 
            (current->next_airport->next_airport != NULL)) {
            current = current->next_airport; 
        }
        if (strcmp(current->next_airport->code, bypass_code) == 0) {  
            existed_code = TRUE;
            // Give a pointer to the current->next_airport->next_airport
            struct airport *new_next_airport = current->next_airport->next_airport;
            if (current->next_airport == flight->dest_airport) {
                flight->dest_airport = current;
            }
            // Free the manifest and people
            delete_manifest(current->next_airport->manifest);
            // Free the airport
            free(current->next_airport);
            current->next_airport = new_next_airport;
        }
    }
    if (existed_code == FALSE) {
        printf("No airport of that code exists!\n");
    }
}

// Stage 3.1
// This function implements the emergency command:
// Reads in emergency hours and then calculates the emergency time.
// This stage will implement an emergency landing at a new airport "ALT0"
// at emergency time.
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_emergency(struct flight *flight) {
    // Read number of emergency hours using fgets
    printf("How long until emergency: ");
    char hours[MAX_STRING_LEN] = {0};
    fgets(hours, MAX_STRING_LEN, stdin);

    // Convert the string to a number
    int emergency_hours = 0;
    emergency_hours = atoi(hours);

    // Calculate the emergency time
    int emergency_time = 0;
    emergency_time = flight->origin_airport->departure_time + emergency_hours * 100;

    if (emergency_hours == 0) {
        flight->dest_airport = flight->origin_airport;
        flight->origin_airport->departure_time = 0;
        // All airports except the origin should be removed
        while (flight->origin_airport->next_airport != NULL) {
            struct airport *new_next_airport = 
            flight->origin_airport->next_airport->next_airport;
            // Free the list of people 
            // Free the manifest
            delete_manifest(flight->origin_airport->next_airport->manifest);
            // Free the airport
            free(flight->origin_airport->next_airport);
            flight->origin_airport->next_airport = new_next_airport;
        }
    } else if (flight->dest_airport->arrival_time < emergency_time) {
        printf("Flight was safe!\n");
    } else { 
        // Implement the emergency landing
        airport_emergency(flight, emergency_time);
    }
}

// Stage 3.1
// This function helps implement the emergency command:
// When the emergency happens, adds an emergency airport "ALT0" at the 
// emergency time and the airports after "ALT0" should be deleted.
// Parameters:
//     flight = the flight which contains the flight route
//     emergency_time = the time of the emergency
// Returns:  
//     None
void airport_emergency(struct flight *flight, int emergency_time) {
    // Provides the emergency airport information
    char emergency_code[MAX_STRING_LEN] = "ALT0";
    int emergency_arrival_time = emergency_time;
    int emergency_departure_time = 0;

    // Give a pointer to the "head" of the flight(flight->origin_airport)
    struct airport *current = flight->origin_airport;

    // Create the emergency airport
    struct airport *emergency_airport  = create_airport(emergency_code, 
    emergency_arrival_time, emergency_departure_time);

    // Set the emergency airport as the destination(flight->dest_airport)
    flight->dest_airport = emergency_airport;

    // Free the airports after the emergency airport(flight->dest_airport)
    while (current != NULL) { 
        if (current->next_airport->arrival_time >= emergency_time) {
            flight->dest_airport->next_airport = current->next_airport;
            current->next_airport = flight->dest_airport;
            // Remove all airports after the landing
            while (flight->dest_airport->next_airport != NULL) {
                struct airport *new_next_airport = 
                flight->dest_airport->next_airport->next_airport;
                // Free the list of people 
                // Free the manifest
                delete_manifest(flight->dest_airport->next_airport->manifest);
                // Free the airport
                free(flight->dest_airport->next_airport);
                flight->dest_airport->next_airport = new_next_airport;
            }
            current = flight->dest_airport;
        }
        current = current->next_airport;
    }
}

// Stage 3.2 
// Helper function
// This function implement cancel command:
// This function use free() and the function delete manifest.c
// to free people, manifets, airports, flight
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_cancel(struct flight *flight) {
    struct airport *current = flight->origin_airport;
    while (current != NULL) {
        struct airport *next_one = current->next_airport;
        // Free the list of people 
        // Free the manifest
        delete_manifest(current->manifest);
        // Free the airport
        free(current);
        current = next_one;
    }
    // Free the flight
    free(flight);
}

// Stage 3.3
// This function implements the reverse command:
// Reverses the flight route.
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     flight(reversed) = the flight which contains the flight route
struct flight *do_reverse(struct flight *flight) {
    if (flight->origin_airport->next_airport == NULL) {
        // If there is only one airport in the list, 
        // no need to reverse
        return flight;
    }
    
    struct airport *head = flight->origin_airport;
    struct airport *previous = NULL;
    // Set the origin as the destination(flight->dest_airport)
    flight->dest_airport = head;
    struct airport *current = head;
    while (current != NULL) {
        struct airport *next_one = current->next_airport;
        current->next_airport = previous;
        previous = current;
        current = next_one;
    }
    // Set the airport previous as the flight->origin_airport
    // since the previous becomes the "head" of the flight after reversing the flight
    flight->origin_airport = previous;
    return flight;
}

// Stage 4.1
// This function implements add_person function:
// Adds person to the airport by using the ADT "Manifest" by 
// implementing the function add_person() from the manifest.c .
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_add_person(struct flight *flight) {
    // Read airport code, passenger name, passenger weight
    // using fgets
    printf("Enter the airport code: ");
    char airport_code[MAX_STRING_LEN] = {0};
    fgets(airport_code, MAX_STRING_LEN, stdin);
    remove_newline(airport_code);

    printf("Enter the passenger name: ");
    char name[MAX_STRING_LEN] = {0};
    fgets(name, MAX_STRING_LEN, stdin);
    remove_newline(name);

    printf("Enter the passenger weight: ");
    char weight_str[MAX_STRING_LEN] = {0};
    fgets(weight_str, MAX_STRING_LEN, stdin);
    remove_newline(weight_str);
    
    // Convert the string to a number
    double weight = 0.00;
    weight = atof(weight_str);
    
    // Give a pointer to the "head"(flight->origin_airport) of the flight
    struct airport *current = flight->origin_airport;
    int existed_code = FALSE;
    while (current != NULL) {
        if (strcmp(current->code, airport_code) == 0) {
            existed_code = TRUE;
            // use the function from manifest.c to add the person 
            // to the manifest of the airport
            add_person(current->manifest, name, weight);
        }
        current = current->next_airport;
    }
    if (existed_code == FALSE) {
        printf("No airport of that code exists!\n");
    }
}

// Stage 4.2
// This function implements print_manifest command:
// Inputs a code and uses the while loop to find the desired airport code.
// Prints all the info about manifest(passenger information) 
// of the desired airport.
// For example: weight - passenger
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_print_manifest(struct flight *flight) {
    // use the variable "existed_code" to 
    // show that the airport ICA0 code exists or not
    // existed_code = FALSE: The code does not exist.
    // existed_code = TRUE: The code exists.
    int existed_code = FALSE;

    // Read airport code using fgets
    printf("Enter the airport code: ");
    char airport_code[MAX_STRING_LEN] = {0};
    fgets(airport_code, MAX_STRING_LEN, stdin);
    remove_newline(airport_code);

    // Give a pointer to the "head"(flight->origin_airport) of the flight
    struct airport *current = flight->origin_airport;
    while (current != NULL) {
        if (strcmp(current->code, airport_code) == 0) {
            // Find the desired airport 
            // and then print its manifest
            existed_code = TRUE;
            print_manifest(current->manifest);
        }
        current = current->next_airport;
    }
    if (existed_code == FALSE) {
        printf("No airport of that code exists!\n");
    } 
}

// Stage 4.3
// This function implements the flight stats:
// Prints the people information in desired order.
// Calculates the total weight of everyone.
// Prints the total weight
// Parameters:
//     flight = the flight which contains the flight route
// Returns:  
//     None
void do_flight_stats(struct flight *flight) {

    // Give a pointer to the "head"(flight->origin_airport) of the flight
    struct airport *current = flight->origin_airport;
    // move people to the destination airport(flight->dest_airport->manifest)
    while (current->next_airport != NULL) {
        // move the people from current->manifest to the
        // current->next_airport->manifest
        current->next_airport->manifest = 
        join_manifest(current->manifest, current->next_airport->manifest);
        current = current->next_airport;
    }
    // Compute the total weight of people
    double flight_weight = 0.0;
    flight_weight = manifest_weight(flight->dest_airport->manifest);

    // Print people in desired order and then print the total weight
    printf("Final manifest:\n");
    print_manifest(flight->dest_airport->manifest);
    printf("Total weight: %lf\n", flight_weight);
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  HELPER FUNCTIONS  //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Helper Function
// You likely do not need to change this function.
//
// Given a raw string will remove and first newline it sees.
// The newline character wil be replaced with a null terminator ('\0')
// Parameters:
//     flight  = the flight to move people along of
// Returns:
//     None
void remove_newline(char input[MAX_STRING_LEN]) {

    // Find the newline or end of string
    int index = 0;
    while (input[index] != '\n' && input[index] != '\0') {
        index++;
    }
    // Goto the last position in the array and replace with '\0'
    // Note: will have no effect if already at null terminator
    input[index] = '\0';
}


// Helper Function
// You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
//
// Given a raw string in the following foramt: [integer0] [integer1] [string]
// This function will extract the relevant values into the given pointer variables.
// The function will also remove any newline characters.
//
// For example, if given: "0135 0545 YSSY"
// The function will put the integer values
//     135 into the val1 pointer
//     545 into the val2 pointer
// And will copy a null terminated string
//     "YSSY" into the 'word' array
//
// If you are interested, `strtok` is a function which takes a string,
// and splits it up into before and after a "token" (the second argument)
//
// Parameters:
//     buffer  = A null terminated string in the following format
//               [integer0] [integer1] [string]
//     val0    = A pointer to where [integer0] should be stored
//     val1    = A pointer to where [integer1] should be stored
//     word    = An array for the [string] to be copied into
// Returns:
//     None
void interpret_line(
    char buffer[MAX_STRING_LEN],
    int *val0,
    int *val1,
    char word[MAX_STRING_LEN]
) {

    // Remove extra newline
    remove_newline(buffer);

    // Extract value 1 as int
    char *val0_str = strtok(buffer, " ");
    if (val0_str != NULL) {
        *val0 = atoi(val0_str);
    } else {
        *val0 = -1;
    }

    // Extract value 2 as int
    char *val1_str = strtok(NULL, " ");
    if (val1_str != NULL) {
        *val1 = atoi(val1_str);
    } else {
        *val1 = -1;
    }

    char *word_str = strtok(NULL, " ");
    if (word_str != NULL) {
        // Extract word
        strcpy(word, word_str);
    }

    if (val0_str == NULL || val1_str == NULL || word_str == NULL) {
        // If any of these are null, there were not enough words.
        printf("Could not properly interpret line: %s.\n", buffer);
    }

}

void do_print_help(void) {
    printf("+-----------------+-------------------------------------------+\n");
    printf("|Command Name     | How to Use                                |\n");
    printf("+=================+===========================================+\n");
    printf("|PRINT ROUTE      | Enter command: print                      |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|SUBROUTE         | Enter command: subroute                   |\n");
    printf("|                 | Enter airport code: [AIRPORT CODE]        |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|CHANGE ORIGIN    | Enter command: change_origin              |\n");
    printf("|                 | Enter new origin info: [ARRIVAL TIME]...  |\n");
    printf("|                 |     ... [DEPARTURE TIME] [AIRPORT CODE]   |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|BYPASS           | Enter command: bypass                     |\n");
    printf("|                 | Enter airport code: [AIRPORT CODE]        |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|EMERGENCY        | Enter command: emergency                  |\n");
    printf("|                 | How long until emergency: [TIME IN HOURS] |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|CANCEL           | Enter command: cancel                     |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|REVERSE          | Enter command: reverse                    |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|                 | Enter command: add_person                 |\n");
    printf("|ADD PERSON       | Enter the airport code: [AIRPORT CODE]    |\n");
    printf("|                 | Enter the passenger name: [NAME]          |\n");
    printf("|                 | Enter the passenger weight: [WEIGHT]      |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|PRINT MANIFEST   | Enter command: print_manifest             |\n");
    printf("|                 | Enter the airport code: [AIRPORT CODE]    |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|FLIGHT STATISTICS| Enter command: stats                      |\n");
    printf("+-----------------+-------------------------------------------+\n");
}