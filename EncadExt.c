#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#define MAX_NOME 100
#define MAX_SIZE 7
#define MAX_SIZEAUX 5
struct cliente{
    char nome[MAX_NOME];
    int id;
    int status;
    int proximo;
};typedef struct cliente Client;

struct lista{
    Client cliente;
    struct lista *prox;
};typedef struct lista Lista;

Lista * tabelaHash[MAX_SIZE];
int proximos[MAX_SIZEAUX] = {-1 , -1 , -1};
int indiceremov[MAX_SIZEAUX] = {-1 , -1 , -1};
int destinoremov[MAX_SIZEAUX] = {-1 , -1 , -1};
int destinos[MAX_SIZEAUX] = {-1, -1 , -1};
//int quant_elems = 4;
int hash(int id){
    return id % MAX_SIZE;
}
int removtracker = 0;

void init_tabela_hash(){
    for(int i = 0; i < MAX_SIZE; i++){
        tabelaHash[i] = NULL;
    }
}

Lista* cria_lista(Client *a){
    Lista *Novalista = (Lista*)malloc(sizeof(Lista));
   // Novalista->cliente = malloc(sizeof(Client*));
    Novalista->cliente.id = (*a).id;
    Novalista->cliente.status = (*a).status;
    Novalista->cliente.proximo = (*a).proximo;
    strcpy(Novalista->cliente.nome,(*a).nome);
    //Novalista->cliente = a;
    Novalista->prox = NULL;
    return Novalista;
}

void add_lista(Client *a, Lista *list){
    Lista **aux = &list;
    while((*aux)->prox != NULL){
        aux = &(*aux)->prox;
    }
    Lista *newnode = cria_lista(a);
    (*aux)->cliente.proximo = (*a).id;
    (*aux)->prox = newnode;
}

void insere_hash(Client *c){
   // int i;
   // quant_elems++;
    int index = hash(c->id);
    if(tabelaHash[index] != NULL){
        add_lista(c,tabelaHash[index]);
    }
    else{
        Lista *newnode = cria_lista(c);
        tabelaHash[index] = newnode;
        int tempid = c->id;
        tabelaHash[index]->cliente.id = tempid;
       // printf("\nTEST = id: %d ", tabelaHash[index]->cliente.id);
       // printf(" TEST = nome: %s\n", tabelaHash[index]->cliente.nome);
    }

}

//fwrite(addressData, sizeData, numbersData, pointerToFile);
void clientesarqmod(){
    FILE *arquivo;
    arquivo = fopen("clientes.dat","wb");
    for(int i = 0; i<MAX_SIZE; i++){
        if(tabelaHash[i] != NULL){
            Lista *aux = tabelaHash[i];
            fwrite(&(tabelaHash[i]->cliente.id), sizeof(int), 1, arquivo);
            fwrite(tabelaHash[i]->cliente.nome,sizeof(char)*MAX_NOME,1,arquivo);
            fwrite(&(tabelaHash[i]->cliente.proximo),sizeof(int),1,arquivo);
            while (aux->prox != NULL)
            {  
                aux = aux->prox;
                fwrite(&(aux->cliente.id),sizeof(int),1,arquivo);
                fwrite(aux->cliente.nome,sizeof(char)*MAX_NOME,1,arquivo);
                fwrite(&(aux->cliente.proximo),sizeof(int),1,arquivo);
                
            }
        }
    }
    fclose(arquivo);
}
//fread(addressData, sizeData, numbersData, pointerToFile);
void clientarqinit(){
    FILE *arquivo;
    arquivo = fopen("clientes.dat","rb");
    //Client *c = malloc(sizeof(Client*));
    int id;
    int prox;
    char *nome = malloc(sizeof(char)*MAX_NOME);
    char *status = malloc(sizeof(char)*6);
    strcpy(status,"false");
   while(!feof(arquivo)){
        fread(&id,sizeof(int),1,arquivo); //esse tava uma bosta tbm ai mudei
        for (int j = 0; j < MAX_NOME; j++)
        {
            char temp;
            fread(&temp, sizeof(char), 1, arquivo);
            nome[j] = temp;
        }
        //strcpy(c->nome,nome);
        fread(&prox,sizeof(int),1,arquivo); //por algum misterio da vida, esse funciona e o de cima nao
        if (nome[0] != '\0'){
            printf("nome: %s \n",nome);
            printf("Codigo do Cliente: %d \n",id);
            printf("proximo: %d \n",prox);
            printf("status: %s \n",status);
            printf("\n");
        }
        }
    fclose(arquivo);
}

void tabelarqinit(){
    FILE *arquivo;
    int indice = 0;
    arquivo = fopen("tabela.dat","wb");
    for(int i = 0; i<MAX_SIZE;i++){
        if (tabelaHash[i] != NULL){
            fwrite(&indice,sizeof(int),1,arquivo);
            indice++;
        }
        else{
            int nada = -1;
            fwrite(&nada,sizeof(int),1,arquivo);
        }
    }
    fclose(arquivo);
}

void tablearqread(){
    FILE *arquivo;
    arquivo = fopen("tabela.dat","rb");
    int *indice = malloc(sizeof(int));
    for(int i = 0; i<MAX_SIZE;i++){
        fread(indice,sizeof(int),1,arquivo);
        printf("indice %d contem: %d \n",i, *indice);
    }
    fclose(arquivo);
}

void invoke_tables(){
    clientesarqmod();
    clientarqinit();
    tabelarqinit();
    tablearqread();
}

void remove_hash(int id){
    //quant_elems--;
    int index = hash(id);
    if(tabelaHash[index] != NULL){
        Lista *aux = tabelaHash[index];
        if(tabelaHash[index]->cliente.id == id){
            tabelaHash[index] = tabelaHash[index]->prox;
            printf("Cliente removido \n");
            free(aux);
            return;
        }
        else{
            while(aux->prox != NULL){
                if(aux->prox->cliente.id == id){
                    Lista *aux2 = aux->prox;
                    aux->prox = aux2->prox;
                    aux->cliente.proximo = aux2->cliente.proximo;
                    free(aux2);
                    return;
                }
                aux = aux->prox;
            }
            printf("Cliente nao existe \n");
            //quant_elems++;
        }
    }
    else{
        printf("Cliente nao existe \n");
       // quant_elems++;
    }
    invoke_tables();
}

void add_cliente(){
    Client *new = (Client*)malloc(sizeof(Client));
    printf("Digite o nome do cliente: ");
    scanf("%s",new->nome);
    printf("Digite o codigo do cliente: ");
    scanf("%d",&new->id);
    new->status = 0;
    new->proximo = -1;
    insere_hash(new);
    invoke_tables();
}

void read_clientes(){
    FILE *arquivo;
    arquivo = fopen("clientes.dat","rb");
    //Client *c = malloc(sizeof(Client*));
    int id;
    //int *newid;
    int prox;
    char *nome = malloc(sizeof(char)*MAX_NOME);
    char *status = malloc(sizeof(char)*6);
    //int *id_array = malloc(sizeof(int)*quant_elems);
    strcpy(status,"false");
   while (!feof(arquivo)){
        //newid = malloc(sizeof(int));
        fread(&id,sizeof(int),1,arquivo);
        //printf("id: %d \n",id);
        for (int j = 0; j < MAX_NOME; j++)
        {
            char temp;
            fread(&temp, sizeof(char), 1, arquivo);
            nome[j] = temp;
        }
        //strcpy(c->nome,nome);
        fread(&prox,sizeof(int),1,arquivo); //por algum misterio da vida, esse funciona e o de cima nao
        Client *new = malloc(sizeof(Client*));
        //id_array[i] = id;
        strcpy(new->nome,nome);
        new->id = id;
        new->proximo = prox;
        new->status = 0;
        insere_hash(new);
        }
    fclose(arquivo);
    invoke_tables();
}

void print_hash(){
    for(int i = 0; i<MAX_SIZE; i++){
        if(tabelaHash[i] != NULL){
            Lista *aux = tabelaHash[i];
            printf("Indice %d: \n",i);
            printf("Nome: %s \n",tabelaHash[i]->cliente.nome);
            printf("Codigo do Cliente: %d \n",tabelaHash[i]->cliente.id);
            printf("Proximo: %d \n",tabelaHash[i]->cliente.proximo);
            printf("Status: %d \n",tabelaHash[i]->cliente.status);
            while (aux->prox != NULL)
            {  
                aux = aux->prox;
                printf("Nome: %s \n",aux->cliente.nome);
                printf("Codigo do Cliente: %d \n",aux->cliente.id);
                printf("Proximo: %d \n",aux->cliente.proximo);
                printf("Status: %d \n",aux->cliente.status);
            }
        }
        else{
            printf("Indice %d: \n",i);
            printf("Vazio \n");
        }
    }
}

int main(){
    init_tabela_hash();
    read_clientes();
    int opcao = 0;
    while (opcao != 3)
    {
        //invoke_tables();
        printf("1 - Adicionar cliente \n");
        printf("2 - Remover cliente \n");
        printf("3 - Sair \n");
        scanf("%d",&opcao);
        switch (opcao)
        {
        case 1:
            add_cliente();
            break;
        case 2:
            printf("Digite o codigo do cliente a ser removido: ");
            int id;
            scanf("%d",&id);
            remove_hash(id);
            invoke_tables();
            break;
        case 3:
            break;
        default:
            break;
        }
    }
    return 0;
}