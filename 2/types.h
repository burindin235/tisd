#ifndef TYPES_H
#define TYPES_H

#define MAX_STRING_LEN 15

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char string_t[MAX_STRING_LEN + 1];

// для хранения даты поступления дд.мм.гггг
typedef struct
{
    char Day[2 + 1];
    char Month[2 + 1];
    char Year[4 + 1];
} date_t;

// для хранения типа жилища
typedef enum
{
    AddressType_Home,
    AddressType_Dorm,
    AddressType_Rent
} address_type;

typedef struct
{
    string_t street;
    int house_nubmer;
    int apartment_nubmer;
} home_address;

typedef struct
{
    int dorm_nubmer;
    int room_nubmer;
} dorm_address;

typedef struct
{
    string_t street;
    int house_nubmer;
    int apartment_nubmer;
    double rent_cost;

} rent_address;

// для хранения жилища
typedef union 
{
    home_address home;
    dorm_address dorm;
    rent_address rent;
} student_adress;

typedef struct
{
    string_t surname;
    string_t name;
    string_t group_name;
    char gender;
    int age;
    double avg_grade;
    date_t entry_date;
    address_type adress_kind;
    student_adress address;
} student_t;

typedef struct
{
    double key;
    size_t index;
} students_keys_t;

#endif