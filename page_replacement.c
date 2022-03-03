#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>
#include<stdbool.h>

void fifo(int pages [],int ref_size, int page_frames){
	int p_frames[page_frames];
	int p_replaced[page_frames];
	int i,j,k,l;
	
	char output[9][25];	
	for(i = 0; i < page_frames; i++){
		p_frames[i] = -1;
		p_replaced[i] = -1;
	}
	
	int index_frame = -1;
	int index_replace = -1;
	int hits = 0;
	int faults = 0;

	
	for(i = 0; i < 8; i++){				// first column is to indicate the memory before the reference string is read
		for(j = 0; j < 25; j++){
			output[i][j] = '-';
		}
	}
	for(j = 0; j < 25; j++){        // page fault 
		output[8][j] = 'N';
	}

	for(k = 0; k < ref_size; k++){		// reading the reference string
		int flag = 0;
		int cur_page = pages[k];
		for(i = 0; i < page_frames; i++){
			if(cur_page == p_frames[i]){
				flag = 1;
				break;
			}
		}

		if(flag == 0){       
			faults++;
			output[8][k+1] = 'Y';
			index_frame = (index_frame+1)%page_frames;
			if(p_frames[index_frame] != -1 && faults >4){
				index_replace = (index_replace+1)%page_frames;
				p_replaced[index_replace] = p_frames[index_frame];
			}
			p_frames[index_frame] = cur_page;
		}

		for(i = 0; i < 4; i++){			// updating the 4 page frames
			if(p_frames[i] != -1)
				output[i][k+1] = '0' + p_frames[i];
		}

		for(i = 4; i < 8; i++){			// updating the 4 most recently replaced pages
			if(p_replaced[i-4] != -1)
				output[i][k+1] = '0' + p_replaced[i-4];
		}

	}
	printf("REF_STRING:        ");
	printf("  ");
	for(i = 0; i < ref_size; i++){  // print the reference string first
		printf("%c ",'0'+pages[i]);
	}
	
	for(i = 0; i < 8; i++){          // Printing the page frames after every cycle  and the most recent pages replaced
		printf("\n");
		if(i == 0){
			printf("\n");
			printf("PAGE_FRAMES(4):    ");
		}
		else if(i == 4){
			printf("\n");
			printf("PAGES_REPLACED(4): ");
		}
		else
			printf("                   ");
		for(j = 0; j < 25; j++){
			printf("%c ",output[i][j]);
		}
	}
	
	printf("\n\nPAGE_FAULT:        ");   // Indicates if a page fault has occured (Y) or not (N) in that particular cycle
	for(j = 0; j < 25; j++){
		printf("%c ",output[8][j]);
	}
	printf("\n\n");
	printf("Total number of page faults = %d\n",faults);
	return;
		
}


int findLruIndex(int array[], int size){   // function to find the page that has been least recently used  
	int min = array[0];
	int i;
	int res = 0;
	for(i = 1; i < size; i++){
		if(array[i] < min){
	  		min = array[i];
			res = i;	
		}
	}
	return res;
}


void lru(int pages[], int ref_size, int page_frames){
	int p_frames[page_frames];
	int p_replaced[page_frames];
	int counter_array[page_frames];
	int i,j,k,l;
	
        char output[9][25];

        for(i = 0; i < page_frames; i++){
                p_frames[i] = -1;
                p_replaced[i] = -1;
        }

        int index_replace = -1;
        int faults = 0;

        for(i = 0; i < 8; i++){                         // first column is to indicate the memory before the reference string is read
                for(j = 0; j < 25; j++){
                        output[i][j] = '-';
                }
        }

        for(j = 0; j < 25; j++){        // page fault 
                output[8][j] = 'N';
        }

	int counter = 0;

	for(k = 0; k < ref_size; k++){    // going through each cycle 
		int flag_detected = 0;
		int flag_new = 0;
		int cur_page = pages[k];
		for(i = 0; i < page_frames; i++){
			if(p_frames[i] == cur_page){   // this condition detects if the page already exists in the memory
				counter++;
				counter_array[i] = counter;
				flag_detected = 1;
				flag_new = 1;
				break;
			}
		}

		if(flag_detected == 0){        // if page does not exist in the memory
			for(i = 0; i < page_frames; i++){
				if(p_frames[i] == -1){		// checks if there are already empty slots in the page frame memory
					counter++;
					faults++;
					output[8][k+1] = 'Y';
					p_frames[i] = cur_page;
				      	counter_array[i] = counter;
					flag_new = 1;
					break;	
				}	
			}
		}

		if(flag_new == 0){      // If there is no empty space in the page frame memory, hence a page has to be replaced
			int lru_pos = findLruIndex(counter_array,page_frames);  // find the index of the page that has to be replaced
			faults++;
			output[8][k+1] = 'Y';
			counter++;
			index_replace = (index_replace+1)%page_frames;
			p_replaced[index_replace] = p_frames[lru_pos]; 
			counter_array[lru_pos] = counter;
			p_frames[lru_pos] = cur_page;
		}
		
                for(i = 0; i < 4; i++){                 // updating the 4 page frames
                        if(p_frames[i] != -1)
                                output[i][k+1] = '0' + p_frames[i];
                }

                for(i = 4; i < 8; i++){                 // updating the 4 most recently replaced pages
                        if(p_replaced[i-4] != -1)
                                output[i][k+1] = '0' + p_replaced[i-4];
                }

		
	}

        printf("REF_STRING:        ");
        printf("  ");
        for(i = 0; i < ref_size; i++){  // print the reference string first
                printf("%c ",'0'+pages[i]);
        }

        for(i = 0; i < 8; i++){          // Printing the page frames after every cycle  and the most recent pages replaced
                printf("\n");
                if(i == 0){
			printf("\n");
                        printf("PAGE_FRAMES(4):    ");
                }
                else if(i == 4){
			printf("\n");
                        printf("PAGES_REPLACED(4): ");
                }
                else
                        printf("                   ");
                for(j = 0; j < 25; j++){
                        printf("%c ",output[i][j]);
                }
        }

        printf("\n\nPAGE_FAULT:        ");   // Indicates if a page fault has occured (Y) or not (N) in that particular cycle
        for(j = 0; j < 25; j++){
                printf("%c ",output[8][j]);
        }
        printf("\n\n");
        printf("Total number of page faults = %d\n",faults);
        return;

		
}

void second_chance(int pages[], int ref_size, int page_frames){
	int p_frames[page_frames];
	int p_replaced[page_frames];
	bool second_chance[page_frames];
	
        int i,j,k,l;

        char output[9][25];

        for(i = 0; i < page_frames; i++){
                p_frames[i] = -1;
                p_replaced[i] = -1;
		second_chance[i] = false;
        }
	int index_replace = -1;
        int pointer = 0;
        int faults = 0;

        for(i = 0; i < 8; i++){                         // first column is to indicate the memory before the reference string is read
                for(j = 0; j < 25; j++){
                        output[i][j] = '-';
                }
        }

        for(j = 0; j < 25; j++){        // display page fault 
                output[8][j] = 'N';
        }

	for(k = 0; k < ref_size; k++){
		int cur_page = pages[k];
		int flag = 0;
		for(i = 0; i < page_frames; i++){
			if(cur_page == p_frames[i]){
				flag = 1;
				second_chance[i] = true;
				break;
			}
		}
	
		if(flag == 0){
			faults++; 
			output[8][k+1] = 'Y';
			while(true){
				if(p_frames[pointer] == -1){
					p_frames[pointer] = cur_page;
					pointer = (pointer+1)%page_frames;
					break;
				}

				else if(!second_chance[pointer]){
					index_replace = (index_replace+1)%page_frames;
					p_replaced[index_replace] = p_frames[pointer];
					p_frames[pointer] = cur_page;
					pointer = (pointer+1)%page_frames;
					break;
				}

				else{
					second_chance[pointer] = false;
					pointer = (pointer+1)%page_frames;
				}
			}
		}

                for(i = 0; i < 4; i++){                 // updating the 4 page frames
                        if(p_frames[i] != -1)
                                output[i][k+1] = '0' + p_frames[i];
                }

                for(i = 4; i < 8; i++){                 // updating the 4 most recently replaced pages
                        if(p_replaced[i-4] != -1)
                                output[i][k+1] = '0' + p_replaced[i-4];
                }		
			
	}

        printf("REF_STRING:        ");
        printf("  ");
        for(i = 0; i < ref_size; i++){  // print the reference string first
                printf("%c ",'0'+pages[i]);
        }

        for(i = 0; i < 8; i++){          // Printing the page frames after every cycle  and the most recent pages replaced
                printf("\n");
                if(i == 0){
			printf("\n");
                        printf("PAGE_FRAMES(4):    ");
                }
                else if(i == 4){
			printf("\n");
                        printf("PAGES_REPLACED(4): ");
                }
                else
                        printf("                   ");
                for(j = 0; j < 25; j++){
                        printf("%c ",output[i][j]);
                }
        }

        printf("\n\nPAGE_FAULT:        ");   // Indicates if a page fault has occured (Y) or not (N) in that particular cycle
        for(j = 0; j < 25; j++){
                printf("%c ",output[8][j]);
        }
        printf("\n\n");
        printf("Total number of page faults = %d\n",faults);
        return;

	
}

int main(int argc, char** argv){

	FILE* fp;
	char filename[100];
	printf("Please enter the name of the input file with reference string (It should be in the same directory): ");
	scanf("%[^\n]%*c",filename);

        int algo;
        printf("Enter the page replacement algorithm to be used among the following(type the number): \n1.FIFO\n2.LRU\n3.Second Chance\n");
        scanf("%d",&algo);

	if((fp = fopen(filename,"r")) == NULL){
		printf("The input file does not exits\n");
		exit(1);
	}

	int ref_string[24];
	int i = 0;
	int input;

	while(fscanf(fp,"%d",&input) > 0){
		if(i == 24){
			printf("Found more than 24 page numbers\n");
			exit(1);
		}
		ref_string[i] = input;
		i++;
	}
	fclose(fp);

	if(algo == 1){
		fifo(ref_string,24,4);
	}
	else if (algo == 2){
		lru(ref_string,24,4);
	}
	else{
		second_chance(ref_string,24,4);
	}

	return 0;
}


