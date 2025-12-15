#include "pch.h"
namespace Error
{
	ERROR errors[ERROR_MAX_ENTRY] =
	{
		ERROR_ENTRY(0, "Системная ошибка: Недопустимый код ошибки"),
		ERROR_ENTRY(1, "Системная ошибка: Системный сбой"),
		
		ERROR_ENTRY_NODEF10(2), ERROR_ENTRY_NODEF10(12), ERROR_ENTRY_NODEF10(22), ERROR_ENTRY_NODEF10(32),
		ERROR_ENTRY_NODEF10(42), ERROR_ENTRY_NODEF10(52), ERROR_ENTRY_NODEF10(62), ERROR_ENTRY_NODEF10(72),
		ERROR_ENTRY_NODEF10(82), ERROR_ENTRY_NODEF(92), ERROR_ENTRY_NODEF(93), ERROR_ENTRY_NODEF(94), 
		ERROR_ENTRY_NODEF(95), ERROR_ENTRY_NODEF(96), ERROR_ENTRY_NODEF(97), ERROR_ENTRY_NODEF(98), ERROR_ENTRY_NODEF(99),

		ERROR_ENTRY(100, "Системная ошибка: Параметр -in должен быть задан"),
		ERROR_ENTRY(101, "Системная ошибка: Превышена длина входного параметра"),
		ERROR_ENTRY(102, "Системная ошибка: Ошибка при открытии файла с исходным кодом(-in)"),
		ERROR_ENTRY(103, "Системная ошибка: Ошибка при создании файла протокола(-log)"),
		ERROR_ENTRY_NODEF(104), ERROR_ENTRY_NODEF(105), ERROR_ENTRY_NODEF(106), ERROR_ENTRY_NODEF(107), ERROR_ENTRY_NODEF(108), ERROR_ENTRY_NODEF(109),
		ERROR_ENTRY_NODEF10(110), ERROR_ENTRY_NODEF10(120), ERROR_ENTRY_NODEF10(130), ERROR_ENTRY_NODEF10(140),
		ERROR_ENTRY_NODEF10(150), ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170), ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),

		ERROR_ENTRY(200, "Лексическая ошибка: Недопустимый символ в исходном файле"),
		ERROR_ENTRY(201, "Лексическая ошибка: Неизвестная последовательность символов"),
		ERROR_ENTRY(202, "Лексическая ошибка: Превышен размер таблицы лексем"),
		ERROR_ENTRY(203, "Лексическая ошибка: Превышен размер таблицы идентификаторов"),
		ERROR_ENTRY(204, "Лексическая ошибка: Превышен максимальный размер исходного файла"),
		ERROR_ENTRY(205, "Лексическая ошибка: Отсутствует точка входа main"),
		ERROR_ENTRY(206, "Лексическая ошибка: Обнаружен символ \'\". Возможно, не закрыт строковый литерал"),
		ERROR_ENTRY(207, "Лексическая ошибка: Превышен размер строкового литерала"),
		ERROR_ENTRY(208, "Лексическая ошибка: Недопустимый целочисленный литерал"),
		ERROR_ENTRY(209, "Лексическая ошибка: В объявлении отсутствует ключевое слово type"),
		ERROR_ENTRY_NODEF10(210), ERROR_ENTRY_NODEF10(220), ERROR_ENTRY_NODEF10(230), ERROR_ENTRY_NODEF10(240),
		ERROR_ENTRY_NODEF10(250), ERROR_ENTRY_NODEF10(260), ERROR_ENTRY_NODEF10(270), ERROR_ENTRY_NODEF10(280), ERROR_ENTRY_NODEF10(290),

		ERROR_ENTRY(300, "Семантическая ошибка: Необъявленный идентификатор"),
		ERROR_ENTRY_NODEF(301),
		ERROR_ENTRY(302, "Семантическая ошибка: Обнаружено несколько точек входа main"),
		ERROR_ENTRY(303, "Семантическая ошибка: В объявлении не указан тип идентификатора"),
		ERROR_ENTRY_NODEF(304),
		ERROR_ENTRY(305, "Семантическая ошибка: Попытка переопределения идентификатора"),
		ERROR_ENTRY(306, "Семантическая ошибка: Превышено максимальное количество параметров функции"),
		ERROR_ENTRY(307, "Семантическая ошибка: Слишком много параметров в вызове"),
		ERROR_ENTRY_NODEF(308),
		ERROR_ENTRY(309, "Семантическая ошибка: Несовпадение типов передаваемых параметров"),
		ERROR_ENTRY(310, "Семантическая ошибка: Использование пустого строкового литерала недопустимо"),
		ERROR_ENTRY_NODEF(311), 
		ERROR_ENTRY_NODEF(312),
		ERROR_ENTRY_NODEF(313),
		ERROR_ENTRY(314, "Семантическая ошибка: Типы данных в выражении не совпадают"),
		ERROR_ENTRY(315, "Семантическая ошибка: Недопустимое строковое выражение"),
		ERROR_ENTRY_NODEF(316),
		ERROR_ENTRY(317, "Семантическая ошибка: Деление на ноль или неверное условие"),
		ERROR_ENTRY(318, "Семантическая ошибка: Недопустимое имя переменной (имя занято ключевым словом)"),
		ERROR_ENTRY_NODEF(319), ERROR_ENTRY_NODEF10(320), ERROR_ENTRY_NODEF10(330), ERROR_ENTRY_NODEF10(340),
		ERROR_ENTRY_NODEF10(350), ERROR_ENTRY_NODEF10(360), ERROR_ENTRY_NODEF10(370), ERROR_ENTRY_NODEF10(380), ERROR_ENTRY_NODEF10(390),

		ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500),

		ERROR_ENTRY(600, "Синтаксическая ошибка: Неверная структура программы"),
		ERROR_ENTRY(601, "Синтаксическая ошибка: Не найден список параметров функции"), 
		ERROR_ENTRY_NODEF(602), 
		ERROR_ENTRY_NODEF(603),
		ERROR_ENTRY_NODEF(604), 
		ERROR_ENTRY_NODEF(605), 
		ERROR_ENTRY_NODEF(606), ERROR_ENTRY_NODEF(607), ERROR_ENTRY_NODEF(608), ERROR_ENTRY_NODEF(609),
		ERROR_ENTRY(610, "Синтаксическая ошибка: Неверный арифметический оператор"),
		ERROR_ENTRY(611, "Синтаксическая ошибка: Ошибка в арифметическом выражении"), 
		ERROR_ENTRY(612, "Синтаксическая ошибка: Недопустимая синтаксическая конструкция"),
		ERROR_ENTRY_NODEF(613), 
		ERROR_ENTRY_NODEF(614), 
		ERROR_ENTRY(615, "Синтаксическая ошибка: Ожидался литерал или идентификатор"),
		ERROR_ENTRY_NODEF(616), 
		ERROR_ENTRY(617, "Синтаксическая ошибка: Ошибка в теле цикла"), 
		ERROR_ENTRY(618, "Синтаксическая ошибка: Отсутствует закрывающая фигурная скобка"), 
		ERROR_ENTRY_NODEF(619), ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
		ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680), ERROR_ENTRY_NODEF10(690),
		ERROR_ENTRY_NODEF100(700), ERROR_ENTRY_NODEF100(800), ERROR_ENTRY_NODEF100(900)
	};

	ERROR geterror(int id)
	{
		ERROR error_info;
		if (id > 0 && id < ERROR_MAX_ENTRY)
		{
			error_info.id = errors[id].id;
			strcpy_s(error_info.message, errors[id].message);
			error_info.position.col = -1;
			error_info.position.line = -1;
		}
		else
		{
			error_info.id = errors[0].id;
			strcpy_s(error_info.message, errors[0].message);
		}
		return error_info;
	}

	ERROR geterrorin(int id, int line, int col)
	{
		ERROR error_info;
		if (id > 0 && id < ERROR_MAX_ENTRY)
		{
			error_info.id = errors[id].id;
			error_info.position.col = col;
			error_info.position.line = line;
			strcpy_s(error_info.message, errors[id].message);
			return error_info;
		}
		else
		{
			error_info.id = 0;
			error_info.position.col = -1;
			error_info.position.line = -1;
			strcpy_s(error_info.message, errors[0].message);
			return error_info;
		}
	}
};
