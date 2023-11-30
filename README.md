# esp32_fw

## Protocol

1. 모든 데이터는 `char` 타입
2. UART BAUDRATE `115200`

### ESP -> STM

| Type   | SOF | P_ID | DATA (ex)                               | EOF  | 설명                                           | 주기        |
| ------ | --- | ---- | --------------------------------------- | ---- | ---------------------------------------------- | ----------- |
| Info   | *   | WF^  | MJU_WIFI                                | "\r\n" | 연결된 WiFi ID                                 | 부팅 후 1회 |
| Info   | *   | IP^  | 192.168.1.135                           | "\r\n" | 설정을 위한 ESP32 IP                           | 부팅 후 1회 |
| Info   | *   | BT^  | 78%,01h20m                              | "\r\n" | 잔량 퍼센트, 남은 시간                         | 1분         |
| Widget | *   | DA^  | 2023-11-21,Mon,07:20:11                 | "\r\n" | yyyy-mm-dd,day,hh:mm:ss                        | 30분 |
| Widget | *   | WT^  | Yongin,Clear,25                     | "\r\n" | 도시, 날씨, 기온                               | 1분         |
| Widget | *   | BS^  | 66-4,29405,7,20                   | "\r\n" | 노선 번호, 정류소 번호, 도착 시간1, 도착 시간2 | 30초        |
| Widget | *   | SC^  | AAPL,178.72                             | "\r\n" | 종목 명, 실시간 주식 가격                      | 30초         |
| Widget | *   | SM^  | AAPL,178.72,179.42,,, 180.31            | "\r\n" | 종목 명, 주식 가격 일별 22개 (최근 한달 종가)  | 1일         |
| User   | *   | BU^  | 1                                       | "\r\n" | 버튼 눌림    | Event       |  |

표정렬 : alt + shift + f

## Libaray

Adafruit_NeoPixel
https://github.com/adafruit/Adafruit_NeoPixel
