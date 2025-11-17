/* IPC 키 및 공용 구조체 정의 
 * 즉, 공유할 메모리들을 정의
 * 1. 정현파 데이터(관절의 움직임을 함수로 나타낸 데이터) Sensor to Controller
 * 2. 재활 상태의 판단 결과 Controller to Logger
 * 3. Watchdog이 계속 감시할 Controller의 PID Controller? to Watchdog
 * */
#ifndef IPC_CONFIG_H
#define IPC_CONFIG_H

#include <sys/types.h>
#include <sys/ipc.h>

#define SHM_KEY_SENSOR ((key_t)1234)
#define SEM_KEY_SYNC ((key_t)5678)

typedef struct {
	double joint_angle; // 1. 정현파 데이터 (sensor -> controller로 보낼듯)
	int is_safe; // 2. 재활 상태의 판단 검사
	/*
	 * -1: 오류, watchdog 정지 및 복구가 일어나야하는 상태
	 * 0: OK
	 * 1: 경고
	 */

	pid_t controller_pid; // 3. Watchdog이 감시할 Controller의 PID
	pid_t watchdog_pid; // 4. Controller가 Watchdog에게 heartbit를 보내야 함

} sensor_shm_data_t;

#endif
