clc; close all; clear;

% коэффициенты фильтра

b=[0.00319806247283222151 0 -0.00639612494566444301 0 0.00319806247283222151]; % numerator (числитель)
a=[1 -3.78125206442570505 5.44829440482254412 -3.54414088511948755 0.878834866871245235]; % denominator (знаменатель)

% Тестовые данные

t = [0:1:39];

% Генерация синусоиды

amp = 1;
bias = 1;
freq = 500;

x = (amp * sin(2*pi*(freq/16000) * t)) + bias;

y_double = filter(b, a, x); % Выходные (отфильтрованные) данные y в double

% Работа АЦП
%x_12 = round(x * (2^12 - 1)/3.3);

x_12 = fix(x * (2^12 - 1)/3.3);

% STM Фильтрация
y_before_dac = filter(b, a, x_12);

% Смещение
y_plus_2047 = y_before_dac + 2047;

% Работа ЦАП
y_dac = round(y_plus_2047) * 3.3/(2^12-1);

% --- Импорт данных в таблицу для наглядного представления данных

% Из вектор-строки делаем вектор-столбец, для представления в таблице

x = reshape(x, [length(x) 1]);
x_12 = reshape(x_12, [length(x_12) 1]);
y_double = reshape(y_double, [length(y_double) 1]);
y_dac = reshape(y_dac, [length(y_dac) 1]);

data_table = table(x, x_12, y_double, y_dac);

% --- Строим графики

subplot(2,2,1)
plot(x)
title('x - Синусоида')

subplot(2,2,2)
plot(x_12)
title('x\_12 - Синусоида с АЦП')

subplot(2,2,3)
plot(y_double)
title('y\_double - Выход фильтра')

subplot(2,2,4)
plot(y_dac)
title('y\_dac - Выход ЦАП')
