#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h> 
#include "ipc_config.h" 

#define NUM_PROCESSES 3
#define PID_STR_LEN 16

// System V 세마포어 초기화를 위한 union
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// 전역 변수 (자원 해제를 위해)
int g_shmid = -1;
int g_semid = -1;

// 실행할 파일들의 경로 (컴파일된 실행 파일 이름이어야 함)
char *process_paths[NUM_PROCESSES] = {
	"./watchdog",
	"./controller",
	"./sensor_sim"
};

// run_process 함수: fork와 execv를 사용하여 새로운 프로세스를 실행
int run_process(char *path, char *argv[])
{
	pid_t pid = fork();

	if (pid < 0) {
		perror("fork failed");
		return -1;
	} else if (pid == 0) {
		printf("[MAIN] Starting %s...\n", argv[0]);
		if(execv(path, argv) == -1) {
			perror("[MAIN] execv failed");
			exit(EXIT_FAILURE);
		}
	}
	return pid; // 부모 프로세스에서는 자식의 pid를 반환
}

// IPC 자원 정리 함수
void cleanup_ipc_resources() {
    if (g_shmid != -1 && shmctl(g_shmid, IPC_RMID, NULL) < 0) {
        perror("[MAIN] SHM cleanup failed");
    }
    if (g_semid != -1 && semctl(g_semid, 0, IPC_RMID) < 0) {
        perror("[MAIN] Semaphore cleanup failed");
    }
}


int main() {
	printf("--- Rehab Watchdog System Start ---\n");
	
	// 1. IPC 자원 초기화
	// 세마포어 생성 (1개의 세마포어, 뮤텍스 역할)
	if ((g_semid = semget(SEM_KEY_SYNC, 1, IPC_CREAT | 0666)) < 0) {
        perror("[MAIN] semget failed");
        return 1;
    }
    // 세마포어 초기화 (값 1로 설정 -> 뮤텍스처럼 동작)
    union semun arg;
    arg.val = 1;
    if (semctl(g_semid, 0, SETVAL, arg) < 0) {
        perror("[MAIN] semctl SETVAL failed");
        cleanup_ipc_resources();
        return 1;
    }
    // 공유 메모리 생성
    if ((g_shmid = shmget(SHM_KEY_SENSOR, sizeof(sensor_shm_data_t), IPC_CREAT | 0666)) < 0) {
        perror("[MAIN] shmget failed");
        cleanup_ipc_resources();
        return 1;
    }

	printf("[MAIN] IPC Resources initialized (SHM ID: %d, SEM ID: %d)\n", g_shmid, g_semid);
	
	// 2. 프로세스 실행: Watchdog -> Controller -> Sensor 순서로 재정립
	pid_t pids[NUM_PROCESSES];
    
	// 2-1. Watchdog 실행 (pids[0])
	char *watchdog_argv[] = {"watchdog", NULL}; 
	pids[0] = run_process(process_paths[0], watchdog_argv);
	if(pids[0] < 0) { goto error_exit; }

	// 2-2. Controller 실행 (pids[1])
	char *controller_argv[] = {"controller", NULL}; 
	pids[1] = run_process(process_paths[1], controller_argv);
	if(pids[1] < 0) { goto error_exit; }

	// 2-3. Sensor 실행 (pids[2])
	char *sensor_sim_argv[] = {"sensor_sim", NULL};
	pids[2] = run_process(process_paths[2], sensor_sim_argv);
	if(pids[2] < 0) { goto error_exit; }

	printf("[MAIN] All core processes launched. PIDs: Watchdog:%d, Controller:%d, sensor_sim:%d\n",pids[0], pids[1], pids[2]); // 배열 인덱스에 맞게 PID 출력

	// 3. 시스템 감시 및 종료 대기
	int status;
	pid_t terminated_pid;

	// 자식 중 하나가 종료할 때까지 대기
	while ((terminated_pid = wait(&status)) > 0) {
		printf("[MAIN] Child process %d terminated.\n", terminated_pid);
		
		// Watchdog(pids[0])이 종료되면 전체 시스템 종료
		if (terminated_pid == pids[0] && (WIFEXITED(status) || WIFSIGNALED(status))){
			printf("[MAIN] Watchdog terminated. Full system will shutdown.\n");
			break;
		}
	}
	// 4. 시스템 종료 및 IPC 자원 해제
	goto normal_exit;

error_exit:
    fprintf(stderr, "[MAIN] A launch error occurred. Terminating processes...\n");
    // 모든 프로세스 강제 종료
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        if (pids[i] > 0) {
            kill(pids[i], SIGKILL);
        }
    }
    // 종료를 기다림 (waitpid)
    while (wait(NULL) > 0);

normal_exit:
    cleanup_ipc_resources();
	printf("--- System Shutdown Complete ---\n");
	return 0;
}
