#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"


void print_usage(char* argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n -create a new database file\n");
	printf("\t -f - (required) path to database file\n");
	return;
}


int main(int argc, char *argv[])
{
	char* filepath = NULL;
	char* addstring = NULL;
	char* employee_name = NULL;
	char* name = NULL;
	unsigned int worked_hours;
	bool newfile = false;
	bool list = false;
	int c;
	int dbfd = -1;
	struct dbheader_t *header = NULL;
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "nf:a:lr:u:")) != -1) {
		switch (c) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				addstring = optarg;
				break;
			case 'l':
				list = true;
				break;
			case 'r':
				employee_name = optarg;
				break;
			case 'u':
                name = optarg;
                if (optind < argc) {
                    worked_hours = strtoul(argv[optind], NULL, 10);
                    optind++;
                } else {
                    fprintf(stderr, "Option -u requires an additional argument for hours.\n");
                    return -1;
                }
                break;
			case '?':
				printf("Unknown option -%c\n", c);
				break;
			default:
				return -1;
		}
	}

	if (filepath == NULL)
	{
		printf("File path is a required argument\n");
		print_usage(argv);
		return 0;
	}



	if (newfile) {
		dbfd = create_db_file(filepath);
		if (dbfd == STATUS_ERROR) {
			printf("Unable to create database file\n");
			return -1;
		}

		if(create_db_header(dbfd, &header) == STATUS_ERROR) {
			printf("Failed to create database header\n");
			return -1;
		}
	}
	else {
		dbfd = open_db_file(filepath);
		if (dbfd == STATUS_ERROR) {
			printf("Unable to open database file\n");
			return -1;
		}

		if (validate_db_header(dbfd, &header) == STATUS_ERROR) {
			printf("Failed to validate database header\n");
			return -1;
		}
	}


	if(read_employees(dbfd, header, &employees) != STATUS_SUCCESS) {
		printf("Failed to read employees");
		return 0;
	}

	if (addstring != NULL)
	{
		header->count++;
		employees = realloc(employees, header->count * (sizeof(struct employee_t)));
		add_employee(header, employees, addstring);
	}


	if (list) {
		list_employees(header, employees);
	}

	if (employee_name != NULL) {
		if (header->count > 0) {
        	remove_employee(header, employees, employee_name);
    	} else {
        	printf("Database is empty. Cannot remove employee.\n");
    	}
	}

	if (name != NULL && worked_hours) {
		update_hours(header, employees, name, worked_hours);
	}

	output_file(dbfd, header, employees);


	return 0;
}