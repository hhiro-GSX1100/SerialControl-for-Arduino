#ifndef _SerialAnalysis_H_
#define _SerialAnalysis_H_

#include <Arduino.h>

#define USE_Serial Serial
//#define BAUD 9600
#define BAUD 115200
#define COMMAND_MAX 100
#define COMSTR_LENGTH 10
#define VALUE_LENGTH 20
//#define sa_debug
class SerialAnalysis{
private:
	char star_char;
	char end_char;
	char delimiter_char;
	char command[COMMAND_MAX + 1];
	bool enableCommand;
	bool enterCommand;
	int char_pos;
public:
	SerialAnalysis() :star_char('{'), end_char('}'),delimiter_char(','), 
                        char_pos(0),enableCommand(false),enterCommand(false){
          command[COMMAND_MAX + 1] = 0;
	}
	void setup(){
		USE_Serial.begin(BAUD);
		#ifdef sa_debug
			USE_Serial.println("SerialAnalysis setup");
		#endif
	}
	bool check(){
		char ch;
		while(USE_Serial.available()){
			ch = USE_Serial.read();
			if(ch == end_char && enterCommand){
                                command[char_pos] = 0;  //終了文字設定
 				enterCommand = false;
				enableCommand = true;
                                #ifdef sa_debug
                			USE_Serial.println("SerialAnalysis end char");
                		#endif
				return enableCommand;
			}else if(ch == star_char && !enableCommand){
				enterCommand = true;
                                #ifdef sa_debug
                			USE_Serial.println("SerialAnalysis start char");
                		#endif
			}else{
				if(enterCommand && !enableCommand){
					// コマンドが最大長を超えた場合初期化
					if(COMMAND_MAX < char_pos){
						init_command();
					}else{
						command[char_pos] = ch;
						++char_pos;
                                                #ifdef sa_debug
                			          USE_Serial.println(ch);
                		                #endif
					}
				}
			}
		}
		return enableCommand;
	}
	void getCommand(char *m_com, int *m_pin, int *m_state, int *m_value){
		int pos = 0;
		if(enableCommand){
			setCommand(&pos, m_com);
			setValue(&pos, m_pin);
			setValue(&pos, m_state);
			setValue(&pos, m_value);
			//初期化
			init_command();
		}else{
			m_com[0] = 0;
			*m_pin = 0;
			*m_state = 0;
			*m_value = 0;
		}
        //Serial.println(sc->command);
	}
private:
	void init_command(){
		char_pos = 0;
		enterCommand = false;
		enableCommand = false;
                for(int i = 0; i <= COMMAND_MAX + 1; ++i){
                  command[i] = 0;
                }
	}
	void setCommand(int *p_pos, char *ch){
		int cnt = 0;
		while(true){
  			ch[cnt] = command[*p_pos];
			++cnt;
			*p_pos = *p_pos + 1;
			if(command[*p_pos] == delimiter_char || command[*p_pos] == 0 || *p_pos > char_pos){
                          break;
			}			
		}
                ch[cnt] = 0;
                *p_pos = *p_pos + 1;
	}
	void setValue(int *p_pos, int *val){
		int cnt = 0;
                char ch_tmp[VALUE_LENGTH + 1] ={};
		while(true){
  			if('0' <= command[*p_pos] && command[*p_pos] <= '9'){
				ch_tmp[cnt] = command[*p_pos];
			}else if(command[*p_pos] == delimiter_char || command[*p_pos] == 0 || *p_pos > char_pos){
                                break;
                        }else if(cnt > (VALUE_LENGTH + 1)){
                                break;
                        }else{
				ch_tmp[cnt] = '0';
			}
			++cnt;
			*p_pos = *p_pos + 1;
		}
                ch_tmp[cnt] = 0;
		*p_pos = *p_pos + 1;
	        *val = atoi(ch_tmp);
                #ifdef sa_debug
                  USE_Serial.print("ch_tmp:");
                  USE_Serial.println(ch_tmp);
                  USE_Serial.print("val:");
                  USE_Serial.println(*val);
                #endif
                return;
	}
};


#endif // _SerialAnalysis_H_
