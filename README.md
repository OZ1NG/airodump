# Airodump

## 사용법

```bash
syntax: airodump <interface>
sample: airodump mon0
```

## 출력 값

- BSSID
- Beacons
  - 받은 beacon 패킷 개수
- #DATA   
  - 받은 data 패킷 개수
- ENC  
  - WEP인지 OPN인지만 구분 가능... (WEP2여도 WEP로 표시된다... (몰라서..))
- ESSID

## 참고

- [과제](https://gitlab.com/gilgil/sns/-/wikis/dot11-frame/report-airodump)
  - [Create dummy wireless network interface | gilgil 멘토님 블로그](https://gilgil.gitlab.io/2020/09/07/1.html)
  - [Create dummy ethernet network interface | gilgil 멘토님 블로](https://gilgil.gitlab.io/2020/07/23/1.html)

- [802.11 MAC 프레임 종류](http://www.ktword.co.kr/word/abbr_view.php?m_temp1=5509)
- [프레임 제어 필드(FCF)](http://www.ktword.co.kr/word/abbr_view.php?nav=&m_temp1=2320&id=761)
  - [관리 프레임](http://www.ktword.co.kr/word/abbr_view.php?nav=&m_temp1=2318&id=758)
    - [관리 프레임 종류](http://www.ktword.co.kr/word/abbr_view.php?nav=&m_temp1=4898&id=1293)
      - [비콘 프레임](http://www.ktword.co.kr/word/abbr_view.php?nav=&m_temp1=2319&id=1293)
        - [능력 정보(Capability Information)](http://www.ktword.co.kr/word/abbr_view.php?nav=&m_temp1=4905&id=758)
  - [제어 프레임](http://www.ktword.co.kr/word/abbr_view.php?nav=&m_temp1=2324&id=1294)
  - [데이터 프레임](http://www.ktword.co.kr/word/abbr_view.php?nav=&m_temp1=1170&id=1294)