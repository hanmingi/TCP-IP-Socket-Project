# 리눅스 기반 C프로그래밍 프로젝트

# 주제 : TCP/IP 소켓을 이용한 악성코드

# 결과물 : 2개 ( 서버파일, 좀비PC 제어 프로그램)

# 개발 과정
# 좀비 PC 제어 프로그램에 사용되는 명령어 들을 모듈 형태로 제작

# 좀비 PC 제어 프로그램 옵션
# help, cd, ls, rm, download, upload, clear, quit, getinfo, pwd, id, ps
# ransomeware, decryption, screenshot, getroot


# 실행원리
# 서버파일(악성코드)가 피해자의 PC에서 실행되면
# 좀비PC 제어 프로그램을 통하여 피해자의 PC에 접속하여 피해자 PC 제어

# 주의
# 위 악성코드는 tcp/ip 소켓 공부를 위해 제작된 악성코드입니다.
# LAN 상에서만 실행이 가능하며 악용할 시 책임은 사용 본인에게 있습니다.
