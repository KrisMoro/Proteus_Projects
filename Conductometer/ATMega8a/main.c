#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

unsigned char Temp_H = 0,Temp_L = 0,OK_Flag = 0,temp_flag;
 
/*** Инициализация DS18B20 ***/
unsigned char DS18B20_init(void)
{
	PORTC &= ~(1 << PC0); // устанавливаем низкий уровень
	DDRC |= (1 << PC0);
	_delay_us(490);
	DDRC &= ~(1 << PC0);
	_delay_us(68);
	OK_Flag = (PINC & (1 << PC0)); // ловим импульс присутствия датчика
	// если OK_Flag = 0 датчик подключен, OK_Flag = 1 датчик не подключен
	_delay_us(422);
	return OK_Flag;
}
 
/*** Функция чтения байта из DS18B20 ***/
unsigned char read_18b20(void)
{
	unsigned char i;  
	unsigned char dat = 0;
	for(i = 0;i < 8;i++)
	{      
		DDRC |= (1 << PC0);
		_delay_us(2);       
		DDRC &= ~(1 << PC0);
		_delay_us(4);       
		dat = dat >> 1;      
		if(PINC & (1 << PC0))
		{          
			dat |= 0x80;
		}
		_delay_us(62);
	}  
	return dat;
}
 
/*** функция записи байта в DS18B20 ***/
void write_18b20(unsigned char dat)
{
	unsigned char i;   
	for(i = 0;i < 8;i++)
	{
		DDRC |= (1 << PC0);
		_delay_us(2);          
		if(dat & 0x01)
		{
			DDRC &= ~(1 << PC0); 
		}
		else
		{
			DDRC |= (1 << PC0);
		}
		dat = dat >> 1;
		_delay_us(62);
		DDRC &= ~(1 << PC0);
		_delay_us(2);
	}  
}
int main()
{ 
	DDRD = 0xFF;
	DDRB |= (1 << PB0)|(1 << PB1)|(1 << PB2)|(1 << PB3);
	PORTD = 0x00;
	PORTB = 0x00;

	TIMSK2 |= (1 << TOIE2); // разрешение прерывания по таймеру2
	TCCR2B |= (1 << CS21);  // предделитель на 8

	_delay_ms(50);

	unsigned int tempint = 0,tempint1,tempint2,tempint3; // переменные для целого значения температуры
	unsigned int temppoint = 0,temppoint1; // переменные для дробного значения температуры

	sei(); //глобально разрешаем прерывания

	while(1)
	{                      
		if(OK_Flag == 1) // если датчик не ответил
		{
		// ставим прочерки во всех разрядах

		}

		DS18B20_init();        // инициализация DS18B20
		write_18b20(0xCC);     // проверка кода датчика
		write_18b20(0x44);     // запуск температурного преобразования
		_delay_ms(1000);
		DS18B20_init();        // инициализация DS18B20
		write_18b20(0xCC);     // проверка кода датчика
		write_18b20(0xBE);     // считываем содержимое ОЗУ
		Temp_L = read_18b20(); // читаем первые 2 байта блокнота
		Temp_H = read_18b20();
		temp_flag = 1;         // флаг знака температуры равен 1(плюс)

		if(Temp_H &(1 << 3))   // проверяем бит знака температуры на равенство единице
		{          
			signed int tmp;
			temp_flag = 0;      // флаг знака равен 0(минус)
			tmp = (Temp_H << 8) | Temp_L;
			tmp = -tmp;
			Temp_L = tmp;
			Temp_H = tmp >> 8;
		}      

		tempint = ((Temp_H << 4) & 0x70)|(Temp_L >> 4); // вычисляем целое значение температуры
		tempint1 = tempint % 1000 / 100; 
		tempint2 = tempint % 100 / 10; 
		tempint3 = tempint % 10;       
		temppoint = Temp_L & 0x0F; // вычисляем дробное значение температуры
		temppoint = temppoint * 625;       // точность температуры
		temppoint1 = temppoint / 1000;       

	
		
	}
}