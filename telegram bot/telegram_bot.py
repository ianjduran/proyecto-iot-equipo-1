import requests
import telebot
import os

from telebot.types import Message

bot = telebot.TeleBot("2131240436:AAEesff5vTebqaJkdwg3xJdDOjzW-fXTTUo")

# Handler para el comando de Inicio del bot
@bot.message_handler(commands=['start'])
def start_command(message):
   keyboard = telebot.types.InlineKeyboardMarkup()

   keyboard.row(
    telebot.types.InlineKeyboardButton('Luna', callback_data='get-Luna'),
    telebot.types.InlineKeyboardButton('Rambo', callback_data='get-Rambo')
  )
   bot.send_message(
       message.chat.id,
       'Bienvenido a PetBot!\n' +
       'A continuación podrás consultar la información de las mascotas registradas\n' +
       'Si Deseas saber otro comandos o más información usa /help o /about\n' +
       'Selecciona la mascota de la cual deseas saber su información: \n',
       reply_markup=keyboard
   )

# Handler del boton Luna del comando /Start
@bot.callback_query_handler(lambda query: query.data == "get-Luna")
def iq_callback(query):
   id_mascota = "9C E8 76 6E"
   printInfo(query.message,id_mascota)

# Handler del boton Rambo del comando /Start
@bot.callback_query_handler(lambda query: query.data == "get-Rambo")
def iq_callback(query):
   id_mascota = "1A E8 C2 82"
   printInfo(query.message,id_mascota)

# Funcion que realiza un Get Request a la base de datos
def printInfo(message,id_mascota):
    URL = '192.168.1.240'
    port = '3000'
    data = 'http://'+ URL + ':' + port + '/api/getInfo/' + id_mascota
    r = requests.get(data)
    pet = r.json()['data'][0]
    print(pet)
    bot.send_message(
        message.chat.id,
        'Información de ' + pet['nombreMascota'] + '\n' +
        'ID de la mascota: ' + pet['idMascota'] + '\n' +
        'Raza de la mascota: ' + pet['raza'] + '\n' +
        'Intervalo de comidas: ' + str(pet['intervaloComida']) + ' hora(s).\n'
    )

# Handler del comando HELP
@bot.message_handler(commands=['help'])
def help_command(message):
   keyboard = telebot.types.InlineKeyboardMarkup()
   keyboard.add(
       telebot.types.InlineKeyboardButton(
           'Manda un correo!', url='mailto:a00829799@itesm.mx?subject=Ayuda%20-%20PetBot'
       )
   )
   bot.send_message(
       message.chat.id,
       'Bienvenido! Puedes encontrar a continuación una lista de los comandos más importantes:\n' +
       '/start - Realiza peticiones sobre el estado de tus mascotas\n' +
       '/help - Desplegar las opciones del bot\n' + 
       '/about - Información sobre el bot\n' + 
       '/foto - Recibe una foto en tiempo real de lo que ve el dispensador.\n' +
       'Si necesitas ayuda, mandame un correo.',
       reply_markup=keyboard
   )

# Handler del comando About
@bot.message_handler(commands=['about'])
def info_about(message):
    bot.send_message(
	message.chat.id,
	'PETBOT es un dispositivo que se encarga de brindar la comida a las mascotas según las especificaciones de tiempo determinado.\n' + 
	'Esto permite a las personas llevar un registro de las veces que la mascota se acerca a comer al día.\n' +
	'Este bot es un complemento al dispositivo para que el usuario pueda conseguir la informacion mas relevante sobre el estado de su mascota.'
    )

# Handler del comando foto
@bot.message_handler(commands=['foto'])
def foto_command(message):
    bot.send_message(
       message.chat.id,
       'Espera un Momento por favor'
    )
    os.system("libcamera-jpeg -o test.jpg -n")
    bot.send_message(
	message.chat.id,
	'Foto Tomada'
    )
    bot.send_photo(message.chat.id, photo=open('test.jpg', 'rb'))

bot.polling(none_stop=True)
