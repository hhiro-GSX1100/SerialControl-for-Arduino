enum PinType{ PT_DIGITAL, PT_ANALOG_R, PT_ANALOG_W};	//PT_DIGITAL = digitalPin, PT_ANALOG_R = digitalPin + analogRead, PT_ANALOG_W = digitalPin + analogWrite
enum PinState{ PS_OFF, PS_DIGITAL_READ, PS_DIGITAL_WRITE, PS_ANALOG_READ, PS_ANALOG_WRITE };
class PinClass{
private:
	boolean m_useable;
	int m_pin;
	PinType m_pinType;
	PinState m_pinState;
	int m_pinValue;
	unsigned long m_timeSpan;
	unsigned long m_time;
	int tmp_digitalValue;
public:
	//コンストラクタ / Constractor
	PinClass(int pin, PinType pinType, boolean useable)
		:m_pin(pin), m_pinType(pinType), m_useable(useable){
			m_pinState = PS_OFF;
			m_pinValue = 0;
			m_timeSpan = 0;
			tmp_digitalValue = 0;
	}
	boolean getUsable(){
		return m_useable;
	}
	//pin　の番号を取得
	int getPinNo(){
		return m_pin;
	}
	//pin のタイプを取得
	PinType getPinType(){
		return m_pinType;
	}
	//pin　の状態を取得
	PinState getPinState(){
		return m_pinState;
	}
	//pin の状態をセット
	void setPinState(PinState pinstate, int pinValue = 0, unsigned long timeSpan = 0){
		if(!m_useable) return;
		setPinMode(pinstate);
		switch(pinstate){
			case PS_OFF:
				m_pinValue = 0;
				m_timeSpan = 0;
				break;
			case PS_DIGITAL_READ:
				m_pinValue = 0;
				m_timeSpan = 0;
				break;
			case PS_DIGITAL_WRITE:
				if(pinValue == 0){
					m_pinValue = 0;
					m_timeSpan = 0;
					digitalWrite(m_pin, LOW);
				}else{
					m_timeSpan = timeSpan;
					if(timeSpan != 0){
						m_time = millis() + m_timeSpan;
					}
					m_pinValue = 1;
					tmp_digitalValue = HIGH;
					digitalWrite(m_pin, HIGH);
				}
				break;
			case PS_ANALOG_READ:
				if(m_pinType != PT_ANALOG_R) return;
				m_pinValue = 0;
				m_timeSpan = 0;
				break;
			case PS_ANALOG_WRITE:
				if(m_pinType != PT_ANALOG_W) return;
				m_pinValue = pinValue > 255 ? 255 : pinValue;
				analogWrite(m_pin, m_pinValue);
				break;
		}
	}
	int getPinValue(){
		return m_pinValue;
	}
	//戻り値は pin の値
	int execute(){
		if(!m_useable) return m_pinValue;
		switch(m_pinState){
			case PS_OFF:
				return m_pinValue;
				break;
			case PS_DIGITAL_READ:
				return digitalRead(m_pin);
				break;
			case PS_DIGITAL_WRITE:
				if(m_timeSpan != 0){
					if(millis() > m_time){
						tmp_digitalValue = !tmp_digitalValue;
						digitalWrite(m_pin, tmp_digitalValue);
						m_time = millis() + m_timeSpan;
					}
					return tmp_digitalValue;
				}else{
					return m_pinValue;
				}
				break;
			case PS_ANALOG_READ:
				return analogRead(m_pin);
				break;
			case PS_ANALOG_WRITE:
				return m_pinValue;
				break;
			default:
				return m_pinValue;
		}
	}
	void setTimeSpan(unsigned long timespan){
		if(!m_useable) return;
		if(m_pinState == PS_DIGITAL_WRITE)
			m_timeSpan = timespan;
	}
private:
	void setPinMode(PinState pinstate){
		if(!m_useable) return;
		if(m_pinState == pinstate) return;
		m_pinState = pinstate;
		switch(m_pinState){
			case PS_OFF:
				pinMode(m_pin, INPUT);
				break;
			case PS_DIGITAL_READ:
				pinMode(m_pin, INPUT);
				break;
			case PS_DIGITAL_WRITE:
				pinMode(m_pin, OUTPUT);
				break;
			case PS_ANALOG_READ:
				if(m_pinType == PT_ANALOG_R){
					pinMode(m_pin, INPUT);
				}
				break;
			case PS_ANALOG_WRITE:
				if(m_pinType == PT_ANALOG_W){
					pinMode(m_pin, OUTPUT);
				}
				break;
		}
	}
};
