/* 
* IPC키를 사용할 예정
* Controller의 pid를 일정 주기마다 살아있는지 체크
* 만약 Controller가 죽었다고 판단 시
* 비정상 상태, 혹은 정지상태라고 판단
* 그러면 감시 대상(Controller)를 강제종료
* 다시 재실행하여 시스템 복구
*/

/*
* 그러면 Controller에서 여기로 생존신호를 주기적으로 보내야 함
* 받은 생존신호를 기준으로 살았는지 죽었는지 계속 체크
* 만약 죽었다면 Controller를 바로 강제 종료후 바로 재실행
*/

/*
* 생존신호: heartBeat
* 살았는지 죽었는지: isAlive
* heartBeat가 오는 주기: timeout
* 정지상태: hung
*/


