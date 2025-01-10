
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SIZE  20
#define CLIENT 5
#define ORDER 10

struct products {
	char description[20];
	double price;
	int item_count;
	int id;
	int times_requested;
	int items_sold;
	int items_failed;
}products;

void paidi(int* fd_order, int* fd_result, int id_paidi){
	int i;
	int out;
	close(fd_order[0]);
	close(fd_result[1]);
	for (i=0; i<ORDER; i++){
		int id_c = rand()%SIZE;
		write(fd_order[1],&id_c,sizeof(int));
		read(fd_result[0],&out,sizeof(int));
		if (out==1){
			printf("Order No %d was successful for item %d for client with id %d\n",i,id_c,id_paidi);
		}
		else{
			printf("Order No %d was not successful for item %d for client with id %d\n",i,id_c,id_paidi);
		}
		sleep(1);
	}
	close(fd_order[1]);
	close(fd_result[0]);
}

int main(){
	struct products catalog[SIZE];
	int fd_order[2],fd_result[2];
	int i;
	for (i=0;i<SIZE;i++){
		printf("Give the descripition for the %d product\n",i+1);
		scanf("%s",catalog[i].description);
		printf("Give the price for the %d product\n",i+1);
		scanf("%lf",&catalog[i].price);
		catalog[i].item_count=2;
		catalog[i].id=i;
		catalog[i].times_requested=0;
		catalog[i].items_sold=0;
		catalog[i].items_failed=0;
	}
	if (pipe(fd_order) == -1 || pipe(fd_result) == -1){
		printf("Error");
		return -1;
	}
	else{
	for ( int j=0;j<CLIENT; j++ ){
		int x = fork();
		if ( x < 0 ){
			printf("Error fork");
			exit(0);
		}
		if ( x == 0 ){
			//child
			paidi(fd_order,fd_result,j);
			exit(0);
		}
	}
			int k;
			int out; // id proiodos.
			int exists;
			close(fd_order[1]);
			close(fd_result[0]);
			for (k=0; k<CLIENT*ORDER; k++){
				//parent
				read(fd_order[0],&out,sizeof(int));
				catalog[out].times_requested++;
				if ( catalog[out].item_count > 0 ){
					exists=1;
					catalog[out].item_count--;
					printf("The item with name %s exists.Cost is %f.New reserve: %d\n",catalog[out].description,catalog[out].price,catalog[out].item_count);
					catalog[out].items_sold++;
					write(fd_result[1],&exists,sizeof(int));	
				}
				else{
					printf("The item with name %s does not exists.\n",catalog[out].description);
					catalog[out].items_failed++;
					exists=0;
					write(fd_result[1],&exists,sizeof(int));
				}
				sleep(0.5);
			}
			close(fd_order[0]);
			close(fd_result[1]);
			for ( int j=0;j<CLIENT; j++ ){
				wait(NULL);
			}
	} 
	int total_items_sold=0;
	int total_items_failed=0;
	double total_profit=0;
	for (i=0;i<SIZE;i++){
		printf("Descripition for the product: %s\n",catalog[i].description);
		printf("Times requested: %d\n",catalog[i].times_requested);
		printf("items_sold: %d\n",catalog[i].items_sold);
		printf("item_failed: %d\n",catalog[i].items_failed);
		total_items_sold+=catalog[i].items_sold;
		total_items_failed+=catalog[i].items_failed;
		total_profit+=catalog[i].items_sold*catalog[i].price;
	}
	printf("Total orders: %d\n",total_items_sold+total_items_failed);
	printf("Total Items sold: %d\n",total_items_sold);
	printf("Total Items failed: %d\n",total_items_failed);
	printf("Total profit: %f\n",total_profit);

return 0;
}
