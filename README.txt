# 자료구조 실습 팀 프로젝트
# 팀장 : 한민기(Hmgi)
# 팀원 : 홍민기(ghdalsrl0708, hong0708), 이현준(hjjunl), 박영훈(Park-Young-Hun)
# 주제 : 악성코드 제작 (좀비PC)
# 결과물 : 2개 ( 서버파일, 좀비PC 제어 프로그램 )

# 실제 사용 코드 -> Project 코드

# 개발 과정
# 각자 맡은 모듈 제작
# 팀장이 모듈을 MAIN 파일과 결합

# 좀비 PC 제어 프로그램 옵션 개발 인원
# 1. 한민기 -> help, cd, ls, rm, download, upload, clear, quit, getroot, UI design
# 2. 홍민기 -> ransomeware, decryption, screenshot
# 3. 박영훈 -> getinfo, pwd, id
# 4. 이현준 -> ps

# TCP/IP 소켓 연결 -> 한민기

# 서버파일(악성코드) 옵션 개발 인원
# 1. TCP / IP 소켓 연결 -> 한민기
# 2. 파일 소켓 전송 -> 한민기


# 실행 원리
# 서버파일(악성코드)가 피해자의 PC에서 실행되면
# 좀비pc 제어 프로그램을 통하여 피해자의 PC에 접속하여 피해자 PC 제어

# 주의
# 위 악성코드는 tcp / ip 소켓으로 만든 악성코드임으로 LAN 상에서만 실행이 가능하며
# 악용할 시 책임은 사용 본인에게 있습니다.
