#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define MAXLINE 511
#define MAX_SOCK 1024
char *EXIT_STRING = "exit";
char *START_STRING = "�濡 �����Ͽ����ϴ�. ���� ������Դϴ�..\n";
char *start_command1 = "start";
char *start_command2 = "\nGAME START\n";
char welcome[MAXLINE + 1];
char buf[MAXLINE + 1];
char cuf[MAXLINE + 1];
int maxfdp1;
int num_chat = 0;
int max;
int nnbyte;
int mafia, police;
int k, pflag = 0;
char *mfmsg = "����� ���Ǿ��Դϴ�.\n";
char *plcmsg = "����� �����Դϴ�. �㿡 ���Ǿư� ������ �˼� �ִ� ��ȸ�� �ο��˴ϴ�.\n";
char *ctzmsg = "����� �ù��Դϴ�. ���ǾƸ� ã���ּ���\n";
char *morning = "\n\t���� ��ҽ��ϴ�. ���Ǿư� �������� �������ּ���\n";
char *nightmsg = "\t������ ���� ������ �����ּ���: ";
char *vote = "------------------------------------------------------\n\t\t��ǥ�� �����մϴ�.\n\t���Ǿƶ�� �����ϴ� ������� ��ȣ�� �Է��ϼ���\n";
char *voteresult = "------------------------------------------------------\n\t���� ���� ��ǥ�� ���� �����";
char *votenotice = "���� ����ڵ��� �����ϰ� ��ǥ�ϼ���)";
char *userdie = "\t����ڴ� �׾����ϴ�..��ǥ���� 0�� �����ּ���\n";
char *victory = "\t�״� ���Ǿ��Դϴ�.\n\n";
char *nvictory = "\t�״� ���Ǿư� �ƴմϴ�.\n";
char *mafiavictory = "\t�ڡڡڸ��Ǿ��� �¸��Դϴ١ڡڡ�\n";
char *ctzvictory = "\t�ڡڡڽù��� �¸��Դϴ١ڡڡ�\n";
int morningvote[5];
char fbuf[MAXLINE + 1];
clock_t g_startTime, g_endTime;
int clisock_list[MAX_SOCK];
int userid[5];
int nvote[5] = { 0 };
int listen_sock;
int valid = 0;
int re;
int turn = 0;
int gturn = 0;  //���� �� Ƚ��
int n = 0;
int indexx = 0;
int invalid = 0;
char target[MAXLINE + 1];
char dieuser[5];
int getmax();
int sbyte;
char *policedie = "\t������ �׾����ϴ�.\n\t*******�����ϼ���!********\n";
char *policevictory = "\t�ڶȶ��� ������ ���ǾƸ� ��ҽ��ϴ١�\n\n";
char *policelose = "\t��û�� ������ ���ǾƸ� ����ҽ��ϴ�.\n";
char votemsg[MAXLINE + 1];
char vvote[MAXLINE + 1];
char police_nt[MAXLINE + 1];
char *correct = "\t�״� ���Ǿư� �½��ϴ�!\n\n";
char *incorrect = "\t������ �ù��� �׾����ϴ�.\n\n\t���� �Ǿ����ϴ�.\n";
char *pcorrect_command = "\t������ �׾����ϴ�. �ù� ���� �������������� ��Ƴ�������\n";
char *pincorrect_command = "\t�ù��� �׾����ϴ�.\n\n";
char *police_command = "\t������ ���ǾƷ� �ǽɵǴ� ����� �����ϼ���\n";
char *order = "your turn. choice number. ";
char *wait_so = "wait..\n";
char *interrupt = "���������� �����Դϴ�. ������ �ٽ� �����ϼ���.\n";
void addClient(int s, struct sockaddr_in *newcliaddr);
void removeClient(int s);
int tcp_listen(int host, int port, int backlog);
int set_nonblock(int sockfd);
int is_nonblock(int sockfd);
void errquit(char *mesg)
{
	perror(mesg);
	exit(1);
}
int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr;
	int i, j, count, nbyte, accp_sock, clilen = sizeof(struct sockaddr_in);
	int turn = 0;
	int numctz = 3;   //�ù��� ��
	int vtlimit[2] = { 3, 2 };

	if (argc != 2) {
		printf("���� : %s port\n", argv[0]);
		exit(0);
	}
	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
	if (listen_sock == -1)
		errquit("tcp_listen fail");
	if (set_nonblock(listen_sock) == -1)
		errquit("set_nonblock fail");

	for (count = 0; ; count++) {         // ù��° while ����
		if (count == 200000) {
			puts("wait for client");
			fflush(stdout); count = 0;
		}
		accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &clilen);
		if (accp_sock == -1 && errno != EWOULDBLOCK)
			errquit("accept fail");
		else if (accp_sock > 0) {
			clisock_list[num_chat] = accp_sock;
			if (is_nonblock(accp_sock) != 0 && set_nonblock(accp_sock) < 0)
				errquit("set_nonblock fail");
			addClient(accp_sock, &cliaddr);
			send(accp_sock, START_STRING, strlen(START_STRING), 0);
			printf("%d��° ����� �߰�.\n", num_chat);
			sprintf(welcome, "%d�� ������Դϴ�.", num_chat);
			send(clisock_list[num_chat - 1], welcome, strlen(welcome), 0);
		}
		if (num_chat == 5) {         // ���ӽ���
			printf("\n������ �����Ϸ��� start�� �Է����ּ���\n");
			while (fgets(buf, sizeof(buf), stdin) != NULL) {      //��ȸ��(����)�� start�� �Է��ϸ� ����
				nnbyte = strlen(buf);
				if (strstr(buf, start_command1) != NULL) {   //buf�� start�̸�
					srand(time(NULL));
					mafia = rand() % 5;   //0���� 4����
					police = (mafia + 1) % 5;
					for (k = 0; k < num_chat; k++)
						send(clisock_list[k], start_command2, strlen(start_command2), 0);
					while (1) {
						re = 0;
						printf("\n���� ��ҽ��ϴ�.\n");  //���⼭���� �ݺ�
						for (k = 0; k < num_chat; k++)
						{
							send(clisock_list[k], morning, strlen(morning), 0);
							if (k == mafia)    //���Ǿƿ���
								send(clisock_list[k], mfmsg, strlen(mfmsg), 0);
							if (k == police && pflag == 0)   //��������
								send(clisock_list[k], plcmsg, strlen(plcmsg), 0);
							if (k != mafia && k != police)   //�Ϲݽùε鿡��
								send(clisock_list[k], ctzmsg, strlen(ctzmsg), 0);
						}
						for (i = 0; i < num_chat; i++)
							nvote[i] = 0;
						for (k = 0; k < num_chat; k++) {
							send(clisock_list[k], vote, strlen(vote), 0);
							if (gturn != 0)
								send(clisock_list[k], votenotice, strlen(votenotice), 0);   //���� ����� �˸�
						}
						for (i = 0; i < turn; i++) {
							sprintf(dieuser, "* %d ��\n", userid[i]);
							for (k = 0; k < num_chat; k++)
								send(clisock_list[k], dieuser, strlen(dieuser), 0);
						}
						while (1) {

							int flag = 0;
							max = 0;

							if (indexx == 0) {
								printf("��ǥ�� �����ϰڽ��ϴ�.\n\n");
								printf("���� �� Ƚ��: %d\n\n", gturn);
							}

							for (i = 0; i < num_chat; i++) { // �� ��ǥ��
								send(clisock_list[indexx], order, strlen(order), 0);
								for (k = 0; k < num_chat; k++) {
									if (k != indexx) {
										send(clisock_list[k], wait_so, strlen(wait_so), 0);
									}
								}
								while (1) {
									errno = 0;
									1 == 1; //nbyte = recv(clisock_list[i], vvote, MAXLINE, 0);
									nbyte = recv(clisock_list[indexx], vvote, MAXLINE, 0);
									if (strstr(vvote, EXIT_STRING) != NULL) {
										printf("���������� �����Դϴ�.\n");
										for (i = 0; i < num_chat; i++)
											send(clisock_list[i], vvote, strlen(vvote), 0);
										removeClient(i);
									}
									if (nbyte > 1) {
										indexx++; //
										break;   //while
									}
									if (nbyte == 0) {
										removeClient(i);
									}
									else if (nbyte == -1 && errno == EWOULDBLOCK)
										continue;

								}

								if (!strncmp(vvote, "1", 1))
									++nvote[0];
								else if (!strncmp(vvote, "2", 1))
									++nvote[1];
								else if (!strncmp(vvote, "3", 1))
									++nvote[2];
								else if (!strncmp(vvote, "4", 1))
									++nvote[3];
								else if (!strncmp(vvote, "5", 1))
									++nvote[4];
								for (i = 0; i < num_chat; i++)
									printf("%d  ", nvote[i]);
								printf("\n");
							}
							if (indexx == 5) {
								for (i = 0; i < num_chat; i++) {
									if (nvote[i] >= vtlimit[gturn]) {
										flag = 1;
										max = i + 1;
									}
								}
								if (flag == 1)
									break;
								else {
									for (i = 0; i < num_chat; i++)
										nvote[i] = 0;
									indexx = 0;
								}
							}

						}//while�������
						userid[turn] = max;       //���� ����� ��ȣ
						turn++;
						printf("**���Ǿƴ� %d���Դϴ�.**\n", mafia + 1);
						printf("**������ %d���Դϴ�.**\n\n", police + 1);
						printf("���� ���� ��ǥ�� ���� ����� %d���Դϴ�\n", max);
						sprintf(votemsg, "%s %d�� �Դϴ�\n", voteresult, max);
						for (i = 0; i < num_chat; i++) {
							send(clisock_list[i], votemsg, strlen(votemsg), 0);
							if (i == max - 1)
								send(clisock_list[i], userdie, strlen(userdie), 0);
						}
						if (max == mafia + 1) { //���Ǿư� �´� ���
							printf("%s\n ���� ����\n", correct);
							for (i = 0; i < num_chat; i++) {
								send(clisock_list[i], correct, strlen(correct), 0);
								send(clisock_list[i], ctzvictory, strlen(ctzvictory), 0);
							}
							return;     //���� ����
						}
						else {        //�ƴ� ��� (�ù�, ����)

							if (max == police + 1) {   //������ ����
								pflag = 1;
								printf("������ �׾����ϴ�.\n");
								for (i = 0; i < num_chat; i++)
									send(clisock_list[i], policedie, strlen(policedie), 0);//�ƴ϶�� ���� ������ �Ѿ��.
							}
							else {          //�ù��� ����
								numctz--;
								printf("\t���� �ù� ��: %d��\n\n", numctz);
								if (pflag == 1 && numctz == 1) {
									for (i = 0; i < num_chat; i++)
										send(clisock_list[i], mafiavictory, strlen(mafiavictory), 0);
									return;
								}
								for (i = 0; i < num_chat; i++)
									send(clisock_list[i], incorrect, strlen(incorrect), 0);//�ƴ϶�� ���� ������ �Ѿ��.

							}
						}
						printf("���� �Ǿ����ϴ�.\n\n");

						for (i = 0; i < num_chat; i++) {
							if (i == mafia) {   //���Ǿ�����
								send(clisock_list[i], nightmsg, strlen(nightmsg), 0);//���Ǿ� �����϶�� �޽���
								while (1) {
									errno = 0;
									1 == 1; //nbyte = recv(clisock_list[i], vvote, MAXLINE, 0);
									nbyte = recv(clisock_list[mafia], target, MAXLINE, 0);//�޼��� û��
									if (nbyte > 0) {
										indexx++; //
										break;   //while
									}
									if (nbyte == 0) {
										removeClient(i);
										continue;
									}
									else if (nbyte == -1 && errno == EWOULDBLOCK)
										continue;
									if (strstr(buf, EXIT_STRING) != NULL) {
										removeClient(i);
									}
								}

								userid[turn] = atoi(target);
								turn++;
								send(clisock_list[atoi(target) - 1], userdie, strlen(userdie), 0);
								if (atoi(target) - 1 == police) //���Ǿư� ������ ���̴� ���
								{
									pflag = 1;
									send(clisock_list[mafia], pcorrect_command, strlen(pcorrect_command), 0);
									for (i = 0; i < num_chat; i++)
										send(clisock_list[i], policedie, strlen(policedie), 0);
								}
								else { //�ù��� ���̴°��
									send(clisock_list[mafia], pincorrect_command, strlen(pincorrect_command), 0);
									numctz--;
									printf("���� �ù� ��: %d��\n", numctz);
									if (numctz == 1 && pflag == 0) {   // �ù� 1��, ���� ����ִ� ���
										for (i = 0; i < num_chat; i++) {
											if (i == police) {
												sprintf(police_nt, "\tuser%d���� �׾����ϴ�.\n", atoi(target));
												send(clisock_list[i], police_nt, strlen(police_nt), 0);
												send(clisock_list[i], police_command, strlen(police_command), 0);//�������Ը�

												while (1) {
													errno = 0;
													1 == 1; //nbyte = recv(clisock_list[i], vvote, MAXLINE, 0);
													nbyte = recv(clisock_list[police], target, MAXLINE, 0);//�޼��� û��
													if (nbyte > 1) {

														indexx++; //
														break;   //while
													}
													if (nbyte == 0) {
														removeClient(i);
													}
													else if (nbyte == -1 && errno == EWOULDBLOCK)
														continue;
													if (strstr(buf, EXIT_STRING) != NULL) {
														removeClient(i);
													}
												}
												if (atoi(target) - 1 == mafia) {//������ ���ǾƸ� ���� ���
													send(clisock_list[police], victory, strlen(victory), 0);
													for (i = 0; i < num_chat; i++) {
														send(clisock_list[i], policevictory, strlen(policevictory), 0);
														send(clisock_list[i], ctzvictory, strlen(ctzvictory), 0);
													}
													return;    //���Ǿ� ��� ���� ����
												}
												else {     //  ���� �ù� 1���� ���� ���
													userid[turn] = atoi(target);
													for (i = 0; i < num_chat; i++) {
														send(clisock_list[i], policelose, strlen(policelose), 0);
														send(clisock_list[i], mafiavictory, strlen(mafiavictory), 0);
													}
													return;
												}
											}
										}
									}
									if (pflag == 1 && numctz == 1) {   //������ �װ� �ù� 1��
										printf("�ù��� ���� ���Ǿ��� ���� �����ϴ�.\n�ڸ��Ǿ��� �¸���\n���� ����\n");
										send(clisock_list[i], mafiavictory, strlen(mafiavictory), 0);
										return;
									}
								}
							}       //if mafia

						}//for
						re = 0;
						gturn++;
						indexx = 0;//���� ��
					}//�ι�° while
				}  //������ �����մϴ� if
			}// start
		}//num_chat==5 start
		for (i = 0; i < num_chat; i++) {
			errno = 0;
			nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
			if (nbyte == 0) {
				removeClient(i);
				continue;
			}
			else if (nbyte == -1 && errno == EWOULDBLOCK)
				continue;
			if (strstr(buf, EXIT_STRING) != NULL) {
				removeClient(i);
				continue;
			}
			buf[nbyte] = 0;
			for (j = 0; j < num_chat; j++)
				send(clisock_list[j], buf, nbyte, 0);
			printf("%s\n", buf);
		}
	} //for (ù��°)
	return 0;
} //main
void addClient(int s, struct sockaddr_in *newcliaddr) {
	char buf[20];

	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
	printf("new client: %s\n", buf);
	clisock_list[num_chat] = s;
	num_chat++;

}
void removeClient(int s) {
	close(clisock_list[s]);
	if (s != num_chat - 1)
		clisock_list[s] = clisock_list[num_chat - 1];
	num_chat--;
	printf("ä�� ������ 1�� Ż�� ���� �����ڼ� =%d\n", num_chat);
}
int getmax()
{
	int max = listen_sock;
	int i;
	for (i = 0; i < num_chat; i++)
		if (clisock_list[i] > max)
			max = clisock_list[i];
	return max;
}
int tcp_listen(int host, int port, int backlog) {
	int sd;
	struct sockaddr_in servaddr;
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("socket fail");
		exit(1);
	}
	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);
	if (bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind fail");
		exit(1);
	}
	listen(sd, backlog);
	return sd;
}
int set_nonblock(int sockfd) {
	int val;
	val = fcntl(sockfd, F_GETFL, 0);
	if (fcntl(sockfd, F_SETFL, val | O_NONBLOCK) == -1)
		return -1;
	return 0;
}
int is_nonblock(int sockfd) {
	int val;
	val = fcntl(sockfd, F_GETFL, 0);
	if (val & O_NONBLOCK)
		return 0;
	return -1;
}
