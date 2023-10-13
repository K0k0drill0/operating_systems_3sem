#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_STR_LEN 10

typedef long long ll;

const int months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

enum actions{
	REGISTRATION,
	LOGIN,
	EXIT,
	TIME, 
	DATE,
	HOW_MUCH,
	LOGOUT,
	SANCTIONS,

	ok,
	WRONG_STRING,
	WRONG_LOGIN,
	MEMORY_ISSUES,
	WRONG_ACTION,
	WRONG_PIN,
	INVALID_PARAMETER
};

typedef struct {
	char* login;
	int PIN;
	int sanctions;
	int tmp_requests;
} User;

void print_welcome_menu() {
	printf("Welcome to the command line shell!\n");
	printf("You can register or log in to your account.\n");
	printf("Choose an action:\n");
	printf("Enter R to register, L to log in, or E to exit the program.\n");
}

void print_register_menu() {
	printf("Come up with a login and PIN code.\n");
	printf("The login must be no longer than 6 characters and consist of Latin characters and/or numbers.\n");
	printf("he PIN code must be an integer in the base 10 number system, the value of the number must range from 0 to 10000.\n");
}

void print_error(int st) {
	switch (st)
	{
	case WRONG_LOGIN:
		printf("You wrote wrong login. Try again.\n");
		break;
	case WRONG_STRING:
		printf("Wrong string.\n");
		break;
	case MEMORY_ISSUES:
		printf("Can not allocate the memory.\n");
		break;
	case WRONG_ACTION:
		printf("Wrong action. Try again.\n");
		break;
	case WRONG_PIN:
		printf("You wrote wrong pin. Try again.\n");
		break;
	case INVALID_PARAMETER:
		printf("Invalid parameter. Try again.\n");
	default:
		break;
	}
}

bool is_separator(char c) {
	return (c == ' ' || c == '\n' || c == '\t');
}

int get_str(char** str_inp) {
	int arr_max_size = 16;
	*str_inp = (char*)malloc(sizeof(char) * arr_max_size);
	if (*str_inp == NULL) {
		return MEMORY_ISSUES;
	}

	char c;
	c = getchar();
	int ind = 0;
	while (is_separator(c)) {
		c = getchar();
	}

	while (!is_separator(c)) {
		if (ind >= arr_max_size - 1) {
			arr_max_size *= 2;
			char *tmp = (char*)realloc(*str_inp, arr_max_size * sizeof(char));
			if (tmp == NULL) {
				free(str_inp);
				return MEMORY_ISSUES;
			}
			*str_inp = tmp;
		}
		(*str_inp)[ind] = c;
		ind++;
		c = getchar();
	}
	(*str_inp)[ind] = '\0';
	return ok;
}

int get_action() {
	char* action;
	int st = get_str(&action);
	if (st != ok) {
		return st;
	}

	st = 0;
	if (!strcmp(action, "Time")) {
		st = TIME;
	}
	else if (!strcmp(action, "Date")) {
		st = DATE;
	}
	else if (!strcmp(action, "Howmuch")) {
		st = HOW_MUCH;
	} 
	else if (!strcmp(action, "Logout")) {
		st = LOGOUT;
	}
	else if (!strcmp(action, "Sanctions")) {
		st = SANCTIONS;
	}
	else {
		st = WRONG_ACTION;
	}

	free(action);
	return st;
}

int start_menu_act() {
	print_welcome_menu();
	char* c;
	if (get_str(&c) != ok) {
		return MEMORY_ISSUES;
	}

	while (strlen(c) != 1 || (c[0] != 'R' && c[0] != 'L' && c[0] != 'E')) {
		print_error(WRONG_ACTION);
		free(c);
		if (get_str(&c) != ok) {
			return MEMORY_ISSUES;
		}
	}

	char a = c[0];
	free(c);
	switch (a)
	{
	case 'R':
		return REGISTRATION;
		break;
	case 'L':
		return LOGIN;
		break;
	case 'E':
		return EXIT;
		break;
	default:
		break;
	}
	return WRONG_ACTION;
}

int request_login(char** login) {
	printf("Login: ");
	if (get_str(login) != ok) {
		return MEMORY_ISSUES;
	}

	if (strlen(*login) > 6) {
		return WRONG_LOGIN;
	}
	return ok;
}

int request_pin(int* pin) {
	printf("PIN: ");

	char* str;
	int st = get_str(&str);
	if (st != ok) {
		return MEMORY_ISSUES;
	}

	int ind = 0;
	while (str[ind] && ind <= 7) {
		if (!isdigit(str[ind])) {
			free(str);
	        return WRONG_PIN;
	    }
		ind++;
	}

	if (ind > 7) {
		free(str);
		return WRONG_PIN;
	}

	*pin = atoi(str);
	if (*pin < 0 || *pin > 100000) {
		free(str);
		return WRONG_PIN;
	}

	free(str);
	return ok;
}

int registration(User** data_base, int* data_base_tmp_size, int* data_base_max_size) {
	print_register_menu();

	char* login;
	int pin;

	int st = request_login(&login);
	while (st != ok) {
		print_error(st);
		if (st == MEMORY_ISSUES) {
			return MEMORY_ISSUES;
		}
		st = request_login(&login);
	}

	st = request_pin(&pin);
	while (st != ok) {
		print_error(st);
		if (st == MEMORY_ISSUES) {
			free(login);
			return MEMORY_ISSUES;
		}
		st = request_pin(&pin);
	}

	//printf("%s, %d\n", login, pin);

	if (*data_base_tmp_size == *data_base_max_size) {
		*data_base_max_size *= 2;
		User* tmp = (User*)realloc(*data_base, *data_base_max_size * sizeof(User));
		if (tmp == NULL) {
			free(data_base);
			return MEMORY_ISSUES;
		}
		*data_base = tmp;
	} 

	(*data_base)[*data_base_tmp_size].login = login;
	// strcpy((*data_base)[*data_base_tmp_size].login, login);
	(*data_base)[*data_base_tmp_size].PIN = pin;
	(*data_base)[*data_base_tmp_size].sanctions = -1;
	(*data_base)[*data_base_tmp_size].tmp_requests = 0;
	(*data_base_tmp_size)++;

	return ok;
}

int login(User* data_base, int data_base_tmp_size, int* tmp_user) {
	char* login;
	int pin;

	int st = request_login(&login);
	while (st != ok) {
		print_error(st);
		if (st == MEMORY_ISSUES) {
			return MEMORY_ISSUES;
		}
		st = request_login(&login);
	}

	st = request_pin(&pin);
	while (st != ok) {
		print_error(st);
		if (st == MEMORY_ISSUES) {
			free(login);
			return MEMORY_ISSUES;
		}
		st = request_pin(&pin);
	}

	for (int i = 0; i < data_base_tmp_size; i++) {
		if (!strcmp(data_base[i].login, login) && data_base[i].PIN == pin) {
			*tmp_user = i;
			free(login);
			return ok;
		}
	}

	free(login);
	return WRONG_LOGIN;
}

int logout() {
	return start_menu_act();
}

int m_time(int* seconds, int* minutes, int* hours, int* day, int* month, int* year) {
	time_t now = time(NULL);
	struct tm *local = localtime(&now);

	*hours = local->tm_hour;
	*minutes = local->tm_min;  
	*seconds = local->tm_sec;
 
	*day = local->tm_mday;
	*month = local->tm_mon + 1;
	*year = local->tm_year + 1900;

	return ok;
}

void print_time(int seconds, int minutes, int hours) {
	printf("Time is %02d:%02d:%02d\n", hours, minutes, seconds);
}

void print_date(int day, int month, int year) {
	printf("Date is: %02d/%02d/%d\n", day, month, year);
}

ll seconds_counter(int year, int month, int day, int hour, int min, int sec) {
	ll ans = 0;
	for (int i = 0; i < month - 1; i++) {
		ans += months[i];
	}
	ans = ans + (day - 1) + (year * 365) + (year / 4 + 1);
	ans = ans * 24 + hour;
	ans = ans * 60 + min;
	ans = ans * 60 + sec;
	return ans;
}

ll flag_counter(ll seconds, char flag) {
	if (flag == 's') return seconds;
	seconds /= 60;
	if (flag == 'm') return seconds;
	seconds /= 60;
	if (flag == 'h') return seconds;
	seconds /= (24 * 365.2425);
	return seconds;
}

int valid_time(char* str) {
	if (strlen(str) != 8) {
		return 0;
	}
	return (isdigit(str[0]) && isdigit(str[1]) && isdigit(str[3]) && isdigit(str[4])
	 && isdigit(str[6]) && isdigit(str[7]) && str[2] == ':' && str[5] == ':');
}

int valid_date(char* str) {
	if (strlen(str) != 10) {
		return 0;
	}
	return (isdigit(str[0]) && isdigit(str[1]) && isdigit(str[3]) && isdigit(str[4])
	 && isdigit(str[6]) && isdigit(str[7]) && str[2] == '/' && str[5] == '/' && 
	isdigit(str[8]) && isdigit(str[9]));
}

int how_much(ll* ans, char* flag_inp) {
	char* time; char* date; char* flag;
	int st1 = get_str(&time);
	int st2 = get_str(&date);
	int st3 = get_str(&flag);

	if (st1 != ok || st2 != ok || st3 != ok) {
		free(time);
		free(date);
		free(flag);
		return MEMORY_ISSUES;
	}

	if (!valid_time(time) || !valid_date(date)) return INVALID_PARAMETER;

	if (strlen(flag) != 2 || (flag[1] != 's' && flag[1] != 'm' && 
	flag[1] != 'h' && flag[1] != 'y') || flag[0] != '-') {
		free(time);
		free(date);
		free(flag);
		return WRONG_ACTION;
	}

	int year1, month1, day1, hour1, min1, sec1, year2, month2, day2, hour2, min2, sec2;

	hour1 = ((int)time[0] - '0') * 10 + (int)time[1] - '0';
	min1 = ((int)time[3] - '0') * 10 + (int)time[4] - '0';
	sec1 = ((int)time[6] - '0') * 10 + (int)time[7] - '0';

	day1 = ((int)date[0] - '0') * 10 + (int)date[1] - '0';
	month1 = ((int)date[3] - '0') * 10 + (int)date[4] - '0';
	year1 = ((int)date[6] - '0') * 1000 + ((int)date[7] - '0') * 100
	 + ((int)date[8] - '0') * 10 + (int)date[9] - '0';

	if (hour1 > 23 || sec1 > 59 || min1 > 59 || month1 > 12 || day1 > 31) return INVALID_PARAMETER;

	m_time(&sec2, &min2, &hour2, &day2, &month2, &year2);

	ll all_sec = seconds_counter(year2, month2, day2, hour2, min2, sec2)
	 - seconds_counter(year1, month1, day1, hour1, min1, sec1);

	*ans = flag_counter(all_sec, flag[1]);
	*flag_inp = flag[1];

	free(time);
	free(date);
	free(flag);
	return ok;
}

int sanctions_checker(User us) {
	if (us.sanctions == -1) return 0;
	return us.tmp_requests >= us.sanctions;
}

int request_pass() {
	char* pass;
	printf("Input the passcode\n");
	if (get_str(&pass) != ok) {
		return MEMORY_ISSUES;
	}

	if (strcmp(pass, "12345")) {
		free(pass);
		return WRONG_PIN;
	}
	else {
		free(pass);
		return ok;
	}
} 

int impose_sanctions(User** data_base, int data_base_tmp_size) {
	char* username;
	char* str_number;

	if (get_str(&username) != ok || get_str(&str_number) != ok) {
		free(username);
		free(str_number);
		return MEMORY_ISSUES;
	}

	int ind = 0;
	while (str_number[ind] != '\0') {
		if (!isdigit(str_number[ind])) {
			free(username);
			free(str_number);
			return INVALID_PARAMETER;
		}
		ind++;
	}

	int st = request_pass();
	if (st != ok) {
		free(username);
		free(str_number);
		return st;
	}

	int num = atoi(str_number);

	for (int i = 0; i < data_base_tmp_size; i++) {
		if (!strcmp((*data_base)[i].login, username)) {
			(*data_base)[i].sanctions = num;

			free(str_number);
			free(username);
			return ok;
		}
	}
	free(str_number);
	free(username);
	return INVALID_PARAMETER;
}

int main() {
	int data_base_max_size = 10;
	int data_base_tmp_size = 0;
	User* data_base;
	data_base = (User*)malloc(sizeof(User) * data_base_max_size);
	if (data_base == NULL) {
		print_error(MEMORY_ISSUES);
		return 1;
	}

	int st;
	int seconds, minutes, hours, day, month, year;

	int tmp_user;

	int q = start_menu_act();

	while (q != EXIT) {
		switch (q) 
		{
		case REGISTRATION:
			st = registration(&data_base, &data_base_tmp_size, &data_base_max_size);
			if (st != ok) {
				free(data_base);
				return 1;
			}
			printf("Registration completed correctly! Please, log in.\n");
			q = LOGIN;
			break;
		case LOGIN:
			st = login(data_base, data_base_tmp_size, &tmp_user);
			if (st == ok) {
				printf("Welcome, %s! Input your action.\n", data_base[tmp_user].login);
				q = get_action();
			}
			else if (st == WRONG_LOGIN) {
				print_error(WRONG_LOGIN);
				q = start_menu_act();
			}
			else {
				q = st;
			}
			break;
		case LOGOUT:
			printf("Successfully logged out. Back to start of the program.\n");
			data_base[tmp_user].tmp_requests = 0;
			q = logout();
			break;
		case TIME:
			if (sanctions_checker(data_base[tmp_user])) {
				printf("The user is under the sanctions! try again.\n");
				printf("Your action: ");
				q = get_action();
				break;
			}

			m_time(&seconds, &minutes, &hours, &day, &month, &year);
			print_time(seconds, minutes, hours);
			data_base[tmp_user].tmp_requests++;
			printf("Your action: ");
			q = get_action();
			break;
		case DATE:
			if (sanctions_checker(data_base[tmp_user])) {
				printf("The user is under the sanctions! try again.\n");
				printf("Your action: ");
				q = get_action();
				break;
			}

			m_time(&seconds, &minutes, &hours, &day, &month, &year);
			print_date(day, month, year);
			data_base[tmp_user].tmp_requests++;
			printf("Your action: ");
			q = get_action();
			break;
		case HOW_MUCH:
			if (sanctions_checker(data_base[tmp_user])) {
				printf("The user is under the sanctions! try again.\n");
				printf("Your action: ");
				q = get_action();
				break;
			}
			ll ans = 0;
			char flag = ' ';
			st = how_much(&ans, &flag);
			if (st != ok) {
				if (st == INVALID_PARAMETER) {
					print_error(INVALID_PARAMETER);
					printf("Your action: ");
					q = get_action();
					break;
				}
				q = st;
				break;
			}
			else {
				printf("There has been %lli %c.\n", ans, flag);
				data_base[tmp_user].tmp_requests++;
			}
			printf("Your action: ");
			q = get_action();
			break;
		case SANCTIONS:
			if (sanctions_checker(data_base[tmp_user])) {
				printf("The user is under the sanctions! try again.\n");
				printf("Your action: ");
				q = get_action();
				break;
			}

			st = impose_sanctions(&data_base, data_base_tmp_size);
			if (st == MEMORY_ISSUES) {
				print_error(st);
				q = st;
			}
			else if (st == INVALID_PARAMETER) {
				print_error(st);
				printf("Your action: ");
				q = get_action();
			}
			else if (st == WRONG_PIN) {
				print_error(WRONG_PIN);
				printf("Your action: ");
				q = get_action();
			}
			else {
				printf("The sanctions was imposed.\n");
				printf("Your action: ");
				q = get_action();
			}
			break;
		case WRONG_ACTION:
			print_error(WRONG_ACTION);
			q = get_action();
			break;
		case MEMORY_ISSUES:
			print_error(MEMORY_ISSUES);
			free(data_base);
			return 1;
			break;
		default:    
			break;
		}
	}
	
	free(data_base);
	return 0;
}