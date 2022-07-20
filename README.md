# Vibration-detection-using-OpenCV
## Описание
Программа анализирует вибрацию объектов по видео
Выдаёт частоту вибрации и амплитуду (если выполнить калибровку с "мерным объектом" в кадре)

---
## Управление
Левый клик - выбор желаемой точки для трекинга (можно выбрать несколько)
Зажатая ПКМ - выделение прямоугольника с автоматическим выбором точек

---
## Блок-схема для калибровки камеры
![camera_calibration](https://user-images.githubusercontent.com/79339489/177723238-94cb40ed-658d-4c5a-a17c-ddad6e9e10ef.png)
---
## Блок-схема для удаления дисторсии
![video_undistortion](https://user-images.githubusercontent.com/79339489/177723500-919874dc-bbba-4503-b179-3d3229cd821e.png)
---
## Блок-схема для трекинга методом Лукаса-Канаде
![lucas_kanade_tracking](https://user-images.githubusercontent.com/79339489/177723572-bf63eddd-801a-422d-8e5f-e5867b2c8400.png)

## АТТЕНШН
тут нет "усиления" движения, его чуть позже добавлю, тк делал его в отдельном проекте
![test1_ampld_3](https://user-images.githubusercontent.com/79339489/179936553-2fefc33e-8f34-4ef4-a9b6-9beea70c7f26.gif)
