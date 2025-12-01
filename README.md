```
rehab_watchog_min/
├── include/
│   └──  ipc_config.h     # IPC 키 및 공용 구조체 정의 (상윤)
|
├── src/
│   ├── main.c           # 시스템 시작 및 프로세스 생성/관리 (상윤)
│   ├── watchdog.c       # 🛡️ 감시 및 복구 로직 (상윤)
│   ├── controller.c     # ⚙️ 핵심 제어 로직 & Heartbeat (서인)
│   └──  sensor_sim.c     # 🌊 모의 센서 데이터 생성 (서인)
│
└── Makefile             # 컴파일 자동화 (상윤)

발표                  # 발표를 합시다! (서인)
```
