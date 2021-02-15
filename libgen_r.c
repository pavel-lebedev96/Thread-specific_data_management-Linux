/*заголовочные файлы*/
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <libgen.h>

/*размеры буфера и аргумента*/
#define MAX_PATH_LEN 256

/*флаг для pthread_once*/
static pthread_once_t thread_once = PTHREAD_ONCE_INIT;

/*ключ для данных уровня потока*/
static pthread_key_t thread_key;

/*функция для освобождения буфера с данными уровня потока*/
static void destructor(void* buf)
{
	free(buf);
}

/*функция для единовременного создания ключа*/
static void create_key(void)
{
	/*На уровне потока выделяем уникальный ключ для буфера
	и сохраняем адрес деструктора этого буфера*/
	if (pthread_key_create(&thread_key, destructor) != 0)
		pthread_exit(1);
}

/*функция для получения адреса буфера для потока*/
static char* get_buf(void)
{
	char* buf;

	if ((buf = pthread_getspecific(thread_key)) == NULL)
	{
		/*Если это первый вызов из данного потока,
		выделяем буфер и сохраняем его адрес*/

		if ((buf = (char*)malloc(MAX_PATH_LEN)) == NULL)
			pthread_exit(1);

		if (pthread_setspecific(thread_key, buf))
			pthread_exit(1);
	}
	return buf;
}

/*реентабельная версия функции dirname*/
char* dirname_r(const char* _path)
{
	char path[MAX_PATH_LEN], * buf;

	/*Копирование аргумента*/
	strncpy(path, _path, MAX_PATH_LEN - 1);
	path[MAX_PATH_LEN - 1] = '\0';

	/*Выделяем ключ для данных уровня потока при первом вызове*/
	if (pthread_once(&thread_once, create_key))
		pthread_exit(1);

	/*Получаем или выделяем адрес для данных потока*/
	buf = get_buf();

	/*Вызов функции и копирование реузльтата в локальный буфер*/
	strncpy(buf, dirname(path), MAX_PATH_LEN - 1);
	buf[MAX_PATH_LEN - 1] = '\0';

	return buf;
}

/*реентабельная версия функции basename*/
char* basename_r(const char* _path)
{
	char path[MAX_PATH_LEN], * buf;

	/*Копирование аргумента*/
	strncpy(path, _path, MAX_PATH_LEN - 1);
	path[MAX_PATH_LEN - 1] = '\0';

	/*Выделяем ключ для данных уровня потока при первом вызове*/
	if (pthread_once(&thread_once, create_key))
		pthread_exit(1);

	/*Получаем или выделяем адрес для данных потока*/
	buf = get_buf();

	/*Вызов функции и копирование реузльтата в локальный буфер*/
	strncpy(buf, basename(path), MAX_PATH_LEN - 1);
	buf[MAX_PATH_LEN - 1] = '\0';

	return buf;
}