# IoT 디바이스에서의 자세추정

1. 아두이노 코드

-Arduino1 : 기본 원시 데이터 값 추출

-Arduino2 : 데이터 값 추출 + 상보필터 + 로우패스 필터

-Arduino3 : 데이터 값 추출 + 상보필터 + 로우패스 필터 + 온도센서 + 파일 저장 기능

2. Processing 프로그램 코드

- Processing.java : 아두이노에서 받은 데이터들을 Serial 통신을 통해 txt파일로 받아 컴퓨터에 저장하게 하는 코드
