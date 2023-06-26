from settings import language

if language == 'en':
    administrator='Administrator'
    adm_close_chatroom = 'Chatroom closed by Administrator'
    user_enter_chatroom='entered the chatroom'
    user_quit_chatroom='quited the chatroom'
    username='username>'
    user_already_exists='username already exists'
    connect_to='connected to'
    connect_from='connected from'
    please_input_username='please input your username'
    out_three_times='input repeat username three times'
    welcome='welcome new user'
elif language =='cn':
    administrator='管理员'
    adm_close_chatroom = '管理员关闭了聊天室'
    user_enter_chatroom='进入了聊天室'
    user_quit_chatroom='离开了聊天室'
    username='用户名>'
    user_already_exists='用户名已经存在'
    connect_to='连接到'
    connect_from='连接来自于'
    please_input_username='请输入用户名'
    #language.pyout_three_times='输入用户名重复超过三次'
    welcome='欢迎新用户'

