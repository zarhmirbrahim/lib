#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// if the compiler is visual c compiler
#ifdef _MSC_VER
	#define gets(x) gets_s(x, sizeof(x))
#endif

#define ENC_KEY 5

///////////////les types//////////////////////
typedef enum Indentifier_t 
{
	ID_CIN,
	ID_CNE, 
	ID_APOGEE 
} Indentifier_t;


typedef union Username_t
{
	char CIN[8];
	char CNE[10];
	int  APOGEE;
} Username_t;


typedef enum UserType_t
{
	UT_STUDENT,
	UT_ADMIN
} UserType_t;


typedef struct User_t
{
	UserType_t	  UType;
	Indentifier_t LogType;
	Username_t	  Login;

	char Password[20];
	char Name[20];
	char Surname[20];
	char Birthday[20];

	struct User_t* NextUser;
} User_t;


typedef struct Book_t
{
	char ISBN[30];
	char Title[50];
	char Author[50];

	struct Book_t* NextBook;
} Book_t;


///////////////////////////////////////////////////////
void EncryptPassword(char* str, int key)
{
	while (*str)
	{
		if (*str >= 'A' && *str <= 'Z')
		{
			*str += key;
			if (*str > 'Z')
				*str -= 26;
		}
		else if (*str >= 'a' && *str <= 'z')
		{
			*str += key;
			if (*str > 'z')
				*str -= 26;
		}
		++str;
	}
}

///////////////////////////////////////////////////
void DecryptPassword(char* str, int key)
{
	while (*str)
	{
		if (*str >= 'A' && *str <= 'Z')
		{
			*str -= key;
			if (*str < 'A')
				*str += 26;
		}
		else if (*str >= 'a' && *str <= 'z')
		{
			*str -= key;
			if (*str < 'a')
				*str += 26;
		}
		++str;
	}
}

/////////////////////////////////////////////////
User_t* g_Users = NULL;
void LoadUsers()
{
	User_t* root = (User_t*)calloc(1, sizeof(User_t));
	if (!root)
		return;
	
	root->UType = UT_ADMIN;
	root->LogType = ID_APOGEE;
	root->Login.APOGEE = 10101;

	strcpy(root->Name, "Root");
	strcpy(root->Password, "root");
	strcpy(root->Birthday, "00-00-0000");
	EncryptPassword(root->Password, ENC_KEY);

	root->NextUser = g_Users;
	g_Users = root;

	FILE* file = fopen("etudiant.bin", "rb");
	if (!file)
		return;

	while (!feof(file))
	{
		User_t* new_user = (User_t*)calloc(1, sizeof(User_t));
		if (!new_user)
			exit(1);

		fread(new_user, sizeof(User_t), 1, file);

		new_user->NextUser = g_Users;
		g_Users = new_user;
	}
	fclose(file);
}

////////////////////////////////////
void SaveUsers()
{
	FILE* file = fopen("etudiant.bin", "wb");
	if (!file)
		return;

	User_t* user = g_Users;

	while (user)
	{
		fwrite(user, sizeof(User_t), 1, file);
		user = user->NextUser;
	}

	fclose(file);
}

/////////////////////////////////////////////////////////
//#define READ_OPT(str) 				\
//	system("cls");					\
//	puts("0- Annuler.");			\
//	puts("1- "#str);				\
//	printf("Entrez votre choix: ");	\
//	scanf("%d", &opt);				\
//	if (opt == 0)					\
//	{								\
//		free(user);					\
//		return;						\
//	}

///////////////////////////////////////////////////////
void READ_OPT(char* str) 				
{			
	printf("1- %s\n", str);				
}
	

void newUserFromConsole()
{
	User_t* user = (User_t*)calloc(1, sizeof(User_t));
	int opt;
	char c;

	READ_OPT("Entrez Le nom et prenom.");
	scanf("%s%s", user->Name, user->Surname);

	READ_OPT("Entrez La date de naissance.");
	scanf("%s", user->Birthday);

	READ_OPT("Entrez 1 pour admin, 2 pour etudiant.");
	scanf("%d", &opt);
	if (opt == 1)
		user->UType = UT_ADMIN;
	else
		user->UType = UT_STUDENT;

	READ_OPT("Entrez 1 pour 'CIN', 2 'CNE' et 3 pour 'APOGEE'.");
	scanf("%d", &opt);
	scanf("%c", &c);

	switch (opt)
	{
	case 1:
		puts("Entrez 'CIN'");
		scanf("%s", user->Login.CIN);
		user->LogType = ID_CIN;
		break;

	case 2:
		puts("Entrez 'CNE''");
		scanf("%s", user->Login.CNE);
		user->LogType = ID_CNE;
		break;

	default:
		puts("Entrez 'APOGEE''");
		scanf("%d", &user->Login.APOGEE);
		user->LogType = ID_APOGEE;
		break;
	}

	READ_OPT("Entrez mot de passe.");
	scanf("%s", user->Password);

	EncryptPassword(user->Password, ENC_KEY);

	user->NextUser = g_Users;
	g_Users = user;
}

///////////////////////////////////////////////////////
Book_t* g_Books = NULL;
int newBooks(FILE* file)
{
	while (!feof(file))
	{
		Book_t* book = (Book_t*)calloc(1, sizeof(Book_t));
		if (!book)
			exit(1);

		fscanf(file, "%s", book->ISBN);
		fscanf(file, "%s", book->Title);
		fscanf(file, "%s", book->Author);

		book->NextBook = g_Books;
		g_Books = book;
	}
	return 1;
}

///////////////////////////////////////////////////////
User_t* User_Auth()
{
	char tmp1[20], tmp[20];
	char c;

	printf("Entrez 'CNE' ou 'CIN' ou 'APOGEE': ");
	gets(tmp1);

	scanf("%c", &c);

	printf("entrez votre mot de passe: ");
	gets(tmp);

	EncryptPassword(tmp, ENC_KEY);

	User_t* user = g_Users;

	while (user)
	{
		if (strcmp(user->Password, tmp) == 0)
		{
			switch (user->LogType)
			{
				case ID_APOGEE:
				if (atoi(tmp1)==user->Login.Apogee)
					return user;
				break;

				case ID_CIN:
				if (strcmp(tmp1, user->Login.CIN)==0)
					return user;
				break;

				case ID_CNE:
				if (strcmp(tmp1, user->Login.CNE)==0)
					return user;
			}
		}
		user = user->NextUser;
	}

	return NULL;
}

///////////////////////////////////////////////////////
void Afficher_Livres()
{
	Book_t* book = g_Books;
	while (book)
	{
		printf("Author: %s\nTitle: %s\n ISBN: %s\n", book->Author, book->Title, book->ISBN);
		book = book->NextBook;
		if (book)
			puts("===========================================");
	}
}

///////////////////////////////////////////////////
void Afficher_Utillisateur()
{
	User_t* user = g_Users;
	while (user)
	{
		printf("Type: ");
		if (user->UType == UT_ADMIN) 
			printf("Admin\n");
		else
			printf("Student\n");

		switch (user->LogType)
		{
		case ID_CIN:
			printf("CIN : %s", user->Login.CIN);
			break;
		case ID_CNE:
			printf("CNE: %s", user->Login.CNE);
			break;
		default:
			printf("Apogee: %d", user->Login.APOGEE);
			break;
		}

		printf("\nPassword :%s\nName: %s\nSurname:%s\nBirthday: %s\n", user->Password, user->Name, user->Surname, user->Birthday);

		user = user->NextUser;
		if (user)
			puts("==================================================");
	}
}


///////////////////////////////////////////////////
void Menu_User()
{
	char tmp[30];
	while (1)
	{
		system("cls");
		puts("================================ Menu ================================");
		puts("1- afficher la liste des livres.");
		puts("2- deconnecter.");
		printf("Entrez votre choix: ");

		int opt;
		scanf("%d", &opt);

		switch (opt)
		{
		case 1:
		{
			system("cls");
			Afficher_Livres();

			scanf("%s", tmp);
			break;
		}
		case 2: return;
		}
	}
}


///////////////////////////////////////////////////
void Menu_Admin()
{
	char tmp[50];
	while (1)
	{
		system("cls");
		puts("================================ Menu ================================");
		puts("1- afficher la liste des utilisateures.");
		puts("2- ajouter un utilisateur.");
		puts("3- afficher la liste des livres.");
		puts("4- deconnecter.");
		printf("Tapez votre choix: ");

		int opt;
		scanf("%d", &opt);

		switch (opt)
		{
		case 1:
		{
			system("cls");
			Afficher_Utillisateur();
			scanf("%s", tmp);
			break;
		}
		case 2:
		{
			system("cls");
			newUserFromConsole();
			scanf("%s", tmp);
			break;
		}
		case 3:
		{
			system("cls");
			Afficher_Livres();

			scanf("%s", tmp);
			break;
		}
		case 4: 
		{
			SaveUsers();
			return;
		}
		}
	}

}

///////////////////////////main/////////////////////
int main()
{
	/////////////////////////////////////////////
	// Lire les etudiants dans un fichier
	/////////////////////////////////////////////

	LoadUsers();

	/////////////////////////////////////////////
	// Lire les etudiants dans un fichier
	/////////////////////////////////////////////
  	FILE* file = fopen("livres.txt", "r");
	if (file)
	{
		newBooks(file);
		fclose(file);
	}

	/////////////////////////////////////////////
	// Lire les information a l'utilisateur
	/////////////////////////////////////////////

	User_t* user = NULL;
	while (user == NULL)
		user = User_Auth();


	/////////////////////////////////////////////
	// Afficher le menu pour admin/user
	/////////////////////////////////////////////

	if (user->UType == UT_STUDENT)
		Menu_User();
	else if (user->UType == UT_ADMIN)
		Menu_Admin();

	return 0;
}
