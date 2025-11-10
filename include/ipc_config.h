/* IPC 키 및 공용 구조체 정의 
 * 즉, 공유할 메모리들을 정의
 * 1. 정현파 데이터(관절의 움직임을 함수로 나타낸 데이터) Sensor to Controller
 * 2. 재활 상태의 판단 결과 Controller to Logger
 * 3. Watchdog이 계속 감시할 Controller의 PID Controller? to Watchdog
 * */

#include <sys/types.h>
#include <sys/ipc.h>

#define SHM_KEY_SENSOR ((key_t)1234)

typedef struct {
	double joint_angle; // 1. 정현파 데이터
	int is_safe; // 2. 재활 상태의 판단 검사
	pid_t controller_pid; // 3. Watchdog이 감시할 Controller의 PID
} sensor_shm_data_t;

