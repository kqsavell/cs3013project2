
#include <stdio.h>
#include <ctype.h>

int scanForVirus(FILE* input); // Returns 0 if virus, -1 if no virus

int main()
{
	FILE *fp;
	fp = fopen("testVirus.txt", "r");
	int ans = scanForVirus(fp);
	printf("ANSWER of testVirus.txt IS: %d\n", ans);
	fclose(fp);

	fp = fopen("testNoVirus.txt", "r");
	ans = scanForVirus(fp);	
	printf("ANSWER of testNoVirus.txt IS: %d\n", ans);
	fclose(fp);
	
	return 0;
}

int scanForVirus(FILE* input) // Returns 0 if virus, -1 if no virus
{
	char c;
	int counter = 0;

	do // Basically increments count if ontrack to spell out 'virus', otherwise reset count
	{
		c = tolower(fgetc(input));

		if(counter == 4 && c == 's')
			return 0;
		else if(counter == 4)
			counter = 0;

		if(counter == 3 && c == 'u')
			counter++;
		else if(counter == 3)
			counter = 0;

		if(counter == 2 && c == 'r')
			counter++;
		else if(counter == 2)
			counter = 0;

		if(counter == 1 && c == 'i')
			counter++;
		else if(counter == 1)
			counter = 0;

		if(counter == 0 && c == 'v')
		{
			counter++;
		}
	}
	while(c != EOF);
	
	return -1;
}
