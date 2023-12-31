#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

int create_db_header(int fd, struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}
	header->version = 0x1;
	header->count  = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;
	return STATUS_SUCCESS;
}


int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->version != 1) {
		printf("Improper header version\n");
		free(header);
		return -1;
	}
	if (header->magic != HEADER_MAGIC) {
		printf("Improper header magic\n");
		free(header);
		return -1;
	}

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Corrupted database\n");
		free(header);
		return -1;
	}
	*headerOut = header;
}


int read_employees(int fd, struct dbheader_t * dbheader, struct employee_t **employeesOut) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	int count = dbheader->count;

	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if(employees == NULL) {
		printf("Malloc failed");
		return STATUS_ERROR;
	}
	read(fd, employees, count * sizeof(struct employee_t));
	int i = 0;
	for(; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	}
	*employeesOut = employees;
	return STATUS_SUCCESS;
}


int add_employee(struct dbheader_t *dbheader, struct employee_t *employees, char* addstring) {
	char *name = strtok(addstring, ",");
	char *address = strtok(NULL, ",");
	char *hours = strtok(NULL, ",");

	strncpy(employees[dbheader->count-1].name, name, sizeof(employees[dbheader->count-1].name));
	employees[dbheader->count-1].name[sizeof(employees[dbheader->count-1].name) - 1] = '\0';
	strncpy(employees[dbheader->count-1].address, address, sizeof(employees[dbheader->count-1].address));
	employees[dbheader->count-1].address[sizeof(employees[dbheader->count-1].address) - 1] = '\0';
	employees[dbheader->count-1].hours = atoi(hours);
	return STATUS_SUCCESS;
}


void output_file(int fd, struct dbheader_t *dbheader, struct employee_t *employees) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return;
	}
	int realcount = dbheader->count;
	dbheader->magic = htonl(dbheader->magic);
	dbheader->version = htons(dbheader->version);
	dbheader->filesize = htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t) * realcount);
	dbheader->count = htons(dbheader->count);

	if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

	if (write(fd, dbheader, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("write");
        return;
    }

	int i = 0;
	for(; i < realcount; i++) {
		employees[i].hours = ntohl(employees[i].hours);
		if (write(fd, &employees[i], sizeof(struct employee_t)) != sizeof(struct employee_t)) {
            perror("write");
            return;
        }
	}

	if (ftruncate(fd, lseek(fd, 0, SEEK_CUR)) == -1) {
        perror("ftruncate");
        return;
    }

	printf("File size after writing: %ld\n", lseek(fd, 0, SEEK_END));
	return;
}

void list_employees(struct dbheader_t *dbheader, struct employee_t *employees) {
	int i = 0;
	for (; i < dbheader->count; i++) {
		printf("Employee %d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours worked: %u\n", employees[i].hours);
	}
	return;
}


void remove_employee(struct dbheader_t *dbheader, struct employee_t *employees, char* employee_name) {
	int counter = dbheader->count;
    int i = 0;
    for (; i < dbheader->count; i++) {
        if (strcmp(employees[i].name, employee_name) == 0) {
            int j;
            for (j = i; j < dbheader->count - 1; j++) {
                strncpy(employees[j].name, employees[j + 1].name, sizeof(employees[j + 1].name));
                strncpy(employees[j].address, employees[j + 1].address, sizeof(employees[j + 1].address));
                employees[j].hours = employees[j + 1].hours;
            }

            dbheader->count--;
            i--;

            printf("Employee '%s' removed successfully.\n", employee_name);
            return;
        }
    }
    if (dbheader->count == counter)
    	printf("Employee '%s' not found.\n", employee_name);
    return;
}



void update_hours(struct dbheader_t *dbheader, struct employee_t *employees, char *employee_name, unsigned int worked_hours) {
    int i = 0;
    unsigned int hrwrkd;
    for (; i < dbheader->count; i++) {
        if (strcmp(employees[i].name, employee_name) == 0) {
            hrwrkd = employees[i].hours;
            employees[i].hours = worked_hours;

            printf("Updated employee %s worked hours from %u to %u\n", employees[i].name, hrwrkd, employees[i].hours);
            return;  // Employee found and updated, exit the function
        }
    }
    printf("Did not find employee %s\n", employee_name);
    return;
}
