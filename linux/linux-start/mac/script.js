time_remaining = 0;
selected_user= null;
valid_image=/.*\.(png|svg|jpg|jpeg|bmp)$/i;

var Lang = navigator.language;

///////////////////////////////////////////////
// CALLBACK API. Called by the webkit greeeter
///////////////////////////////////////////////

// called when the greeter asks to show a login prompt for a user
function show_prompt(text) {
  var password_container= document.getElementById("password_container");
  var password_entry= document.getElementById("password_entry");
  if (!isVisible(password_container)) {
    var users= document.getElementsByClassName("user");
    
    var user_node= document.getElementById(selected_user);
    var rect= user_node.getClientRects()[0];
    var parentRect= user_node.parentElement.getClientRects()[0];
    var center= parentRect.width/2;
    var left= center - rect.width/2 - rect.left;
    if (left < 5 && left > -5) {
      left= 0;
    }
    for (var i= 0; i < users.length; i++) {
      var node= users[i];
      setVisible(node, node.id == selected_user);
//    node.style.left= left;
    }
    user_node.style.left= left;
    setVisible(password_container, true);
    if (Lang == "zh-cn")
      password_entry.placeholder = "密码";
    else 
      password_entry.placeholder= text.replace(":", "");
  }
  password_entry.value= "";
  password_entry.focus();
}

// called when the greeter asks to show a message
function show_message(text) {
  var message= document.getElementById("message_content");
  message.innerHTML= text;
  if (text) {
    document.getElementById("message").classList.remove("hidden");
  } else {
    document.getElementById("message").classList.add("hidden");
  }
  message.classList.remove("error");
}

// called when the greeter asks to show an error
function show_error(text) {
   show_message(text);
   var message= document.getElementById("message_content");
   message.classList.add("error");
}

// called when the greeter is finished the authentication request
function authentication_complete() {
   if (lightdm.is_authenticated){
       clear_screen();
       lightdm.login (lightdm.authentication_user, lightdm.default_session);
   } 
   else {
       if (Lang == "zh-cn")
         show_error("认证失败");
       else  
         show_error("Authentication Failed");
       start_authentication(selected_user);   
    }
}

/////////////////////////
// if is_authenticated which means password is right, clearing screen,if not which means
// Authentication failed doing nothing.
// //////////////////////
function clear_screen(){
  document.body.innerHTML="";    
}


// called when the greeter wants us to perform a timed login
function timed_login(user) {
   lightdm.login (lightdm.timed_login_user);
   setTimeout('throbber()', 1000);
}

//////////////////////////////
// Implementation
//////////////////////////////
function start_authentication(username) {
   lightdm.cancel_timed_login();
   selected_user= username;
   lightdm.start_authentication(username);
}

function provide_secret() {
   if (Lang == "zh-cn")
     show_message("正在登陆...");
   else 
     show_message("Logging in...");
   entry = document.getElementById('password_entry');
   lightdm.provide_secret(entry.value);
}

function show_users() {
  var users= document.getElementsByClassName("user");
  for (var i= 0; i < users.length; i++) {
    setVisible(users[i], true);
    users[i].style.left= 0;
  }
  setVisible(document.getElementById("password_container"), false);
  selected_user= null;
}

function user_clicked(event) {
  if (selected_user != null) {
    selected_user= null;
    lightdm.cancel_authentication();//Cancel the user authentication that is currently in progress.
    show_users();
  } else {
    selected_user= event.currentTarget.id;
    start_authentication(event.currentTarget.id);
	console.log(selected_user);
  }
  document.getElementById("submit_img").className="submit_hidden";
  show_message("");
  event.stopPropagation();
  return false;
}

function setVisible(element, visible) {
  if (visible) {
    element.classList.remove("hidden");
  } else {
    element.classList.add("hidden");
  }
}

function isVisible(element) {
  return !element.classList.contains("hidden");
}

function update_time() {
  var time= document.getElementById("current_time");
  var date= new Date();
  
  var hh = date.getHours();
  var mm = date.getMinutes();
  var ss = date.getSeconds();
  var suffix= "AM";
  if (hh > 12) {
    hh= hh - 12;
    suffix= "PM";
  }
  if (hh < 10) {hh = "0"+hh;}
  if (mm < 10) {mm = "0"+mm;}
  if (ss < 10) {ss = "0"+ss;}
  time.innerHTML= hh+":"+mm + " " + suffix;
}

//////////////////////////////////
// Initialization
//////////////////////////////////

function initialize() {
  show_message("");
  initialize_users();
  initialize_actions();
  initialize_timer();
}

function initialize_users() {
  var template= document.getElementById("user_template");
  var parent= template.parentElement;
  parent.removeChild(template);
  for (i in lightdm.users) {
    user= lightdm.users[i];
    console.log(user.display_name);	
//    alert(user.name);
    userNode= template.cloneNode(true);
    
    var image= userNode.getElementsByClassName("user_image")[0];
    var name= userNode.getElementsByClassName("user_name")[0];
    name.innerHTML= user.display_name;
    
    if (user.image) {
      image.src = "file:///etc/lightdm/face/"+user.name+".face";
      image.onerror= function(e) {
      e.currentTarget.src= "img/avatar.png";
      }
    } else {
      image.src = "img/avatar.png";
    }

    userNode.id= user.name;
    userNode.onclick= user_clicked;
    parent.appendChild(userNode);
  }
  var users= document.getElementsByClassName("user");
  for (var i= 0; i < users.length; i++) {
    console.log(users[i]);	
  }
  setTimeout(show_users, 400);
}

function initialize_actions() {
  var template= document.getElementById("action_template");
  var parent= template.parentElement;
  parent.removeChild(template);
  if (Lang == "zh-cn"){
     add_action("other","其他用户", "img/avatar-more.svg", function(e) {show_other_user_login(); e.stopPropagation();}, template, parent);
     if (lightdm.can_suspend) {
        add_action("sleep","待机", "img/sleep.svg", function(e) {lightdm.suspend(); e.stopPropagation();}, template, parent);
     }
     if (lightdm.can_shutdown) {
        add_action("shutdown", "关机", "img/shutdown.svg", function(e) {lightdm.shutdown(); e.stopPropagation();}, template, parent);
     }
     if (lightdm.can_restart) {
        add_action("restart", "重启", "img/restart.svg", function(e) {lightdm.restart(); e.stopPropagation();}, template, parent);
     }

  }
  else {
     add_action("other","Other", "img/avatar-more.svg", function(e) {show_other_user_login(); e.stopPropagation();}, template, parent);
     if (lightdm.can_suspend) {
        add_action("sleep","Sleep", "img/sleep.svg", function(e) {lightdm.suspend(); e.stopPropagation();}, template, parent);
     }
     if (lightdm.can_shutdown) {
       add_action("shutdown", "Shutdown", "img/shutdown.svg", function(e) {lightdm.shutdown(); e.stopPropagation();}, template, parent);
     }
     if (lightdm.can_restart) {
        add_action("restart", "Restart", "img/restart.svg", function(e) {lightdm.restart(); e.stopPropagation();}, template, parent);
     }

  }
}
function initialize_timer() {
  update_time();
  setInterval(update_time, 1000);
}

function add_action(id, name, image, clickhandler, template, parent) {
  action_node= template.cloneNode(true);
  action_node.id= "action_" + id;
  img_node= action_node.getElementsByClassName("action_image")[0];
  label_node= action_node.getElementsByClassName("action_label")[0];
  label_node.innerHTML= name;
  img_node.src= image;
  action_node.onclick= clickhandler;
  parent.appendChild(action_node);
}

function handle_input() {
  selected_user=document.getElementById("entryUser").value;
  start_authentication(selected_user);
}

function password_input() {
  document.getElementById("submit_img").className="submit";
} 

function update_caps_lock_indicator() {
  //update caps lock indicator by focus event
  //--webkit original bug(only update caps lock indicator by focus envent or caps lock is on )
  var password_entry=document.getElementById("password_entry");
  password_entry.blur();
  password_entry.focus();

}

/*handle key press event.
 *
 * */
function handle_key_press() {
  var keycode =event.keyCode;
  /*keyCode of "Esc" is 27 */
  if (keycode == 27){
		show_users();
		setVisible(password_container,false);
		setVisible(other_users_login,false);
		show_message("");
  }
 /* keyCode of "Enter" is 13 */
  if (keycode == 13){
		if (selected_user != null) {
			submit();
		} else {
			var act = document.activeElement.id;
			if(act == "")
			{
				for (i in lightdm.users){
					users = lightdm.users[i];
				}
				selected_user = users.name;
				start_authentication(users.name);
	//			selected_user = lightdm.users[1].name;
	//			start_authentication(lightdm.users[1].name);
			} else {
				for (i in lightdm.users) {
 					users = lightdm.users[i];
					if(act == users.display_name){
						selected_user = users.name;
						start_authentication(users.name);
	//			} else {
	//				selected_user = lightdm.users[1].name;
	//				start_authentication(lightdm.users[1].name);
					}
				}
			}
    		document.getElementById("submit_img").className="submit_hidden";
 		}		
	}
}

function show_other_user_login() { 
  var password_container=document.getElementById("password_container"); 
  if (selected_user != null){
  selected_user = null;
  lightdm.cancel_authentication();
  }
  document.getElementById("submit_img").className="submit_hidden";
  var users= document.getElementsByClassName("user"); 
  var password_entry= document.getElementById("password_entry");
  password_entry.value= "";
//  alert(selected_user);  
  if (!isVisible(other_users_login)) {  
    for (var i= 0; i < users.length; i++) { 
      setVisible(users[i], false); 
      users[i].style.left= 0; 
    }   
//    setVisible(password_container, false); 
    setVisible(password_container, true); 
    setVisible(other_users_login,true);
    entryUser = document.getElementById("entryUser");
    entryUser.value = "";
    if (Lang == "zh-cn"){
       entryUser.placeholder = "用户名";
       password_entry.placeholder="密码";
    }
    else {
       entryUser.placeholder = text.replace(":","");
       password_entry.placeholder= text.replace(":","");
    }
   // document.getElementById("entryUser").focus(); 
  } else { 
    show_users(); 
    setVisible(password_container,false); 
    setVisible(other_users_login,false);                                       
    } 
}
