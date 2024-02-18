const checkbox__box = document.getElementById('checkbox__box');


// document.addEventListener("visibilitychange", () => {
// if (!loaded && document.visibilityState === 'visible') {
//     console.log("start");
// }
//     console.log("start");
// });
// window.onfocus = () => {
//     // listenerActive = true;
//     // Логика для случая, когда пользователь вернулся
//     console.log("start");
// };

async function get__html() {
    // запрашиваем JSON с данными пользователя
    let response = await fetch('/status');
    if (response.ok) {
        let val = await response.text();
        let jsonObj = JSON.parse(val);
        for (x in jsonObj) {
            let label = document.createElement('label');
            label.className = "checkbox style-e";
            let input = document.createElement('input');
            input.type = "checkbox";
            input.id = jsonObj[x]["name"];
            let checkmark = document.createElement('div');
            checkmark.className = "checkbox__checkmark";
            let text_status = document.createElement('div');
            text_status.className = "checkbox__body";
            text_status.id = `${jsonObj[x]["name"]}__status`;
            text_status.innerHTML = `${jsonObj[x]["name"]} status = ${jsonObj[x]["status"]}`;
            switch (jsonObj[x]["status"]) {
                case 0:
                    input.checked = false;
                    text_status.textContent = 'relay status off';
                    break;
                case 1:
                    input.checked = true;
                    text_status.textContent = 'relay status on';
                    break;
                default:
                    break;
            }
            input.addEventListener('change', switching)
            label.append(input);
            label.append(checkmark);
            label.append(text_status);
            checkbox__box.append(label);
        }
    }
    else {
        document.getElementById('relay__text_status').textContent = 'relay status error';
    }
}
document.addEventListener("DOMContentLoaded", get__html);
// document.addEventListener("DOMContentLoaded", () => {
// (async () => {
//     // запрашиваем JSON с данными пользователя
//     let response = await fetch('/status');
//     if (response.ok) {
//         let val = await response.text();
//         let jsonObj = JSON.parse(val);
//         for (x in jsonObj) {
//             let label = document.createElement('label');
//             label.className = "checkbox style-e";
//             let input = document.createElement('input');
//             input.type = "checkbox";
//             input.id = jsonObj[x]["name"];
//             let checkmark = document.createElement('div');
//             checkmark.className = "checkbox__checkmark";
//             let text_status = document.createElement('div');
//             text_status.className = "checkbox__body";
//             text_status.id = `${jsonObj[x]["name"]}__status`;
//             text_status.innerHTML = `${jsonObj[x]["name"]} status = ${jsonObj[x]["status"]}`;
//             switch (jsonObj[x]["status"]) {
//                 case 0:
//                     input.checked = false;
//                     text_status.textContent = 'relay status off';
//                     break;
//                 case 1:
//                     input.checked = true;
//                     text_status.textContent = 'relay status on';
//                     break;
//                 default:
//                     break;
//             }
//             input.addEventListener('change', switching)
//             label.append(input);
//             label.append(checkmark);
//             label.append(text_status);
//             checkbox__box.append(label);
//         }
//     }
//     else {
//         document.getElementById('relay__text_status').textContent = 'relay status error';
//     }
// })();
// });

async function switching(event) {
    // document.getElementById('relay__text_status').textContent = event;
    // console.log("-------------");
    // console.log(event);
    // console.log(event.type);
    // console.log(event.target);
    // console.log(event.target.id);
    // console.log("-------------");
    let pin, status;
    pin = event.target.id;
    status = "";
    status = event.target.checked;
    switch (status) {
        case true:
            status = 1;
            break;
        case false:
            status = 0;
            break;
        default:
            break;
    }
    let pin__text_status = document.getElementById(`${event.target.id}__status`);
    (async () => {
        // запрашиваем JSON с данными пользователя
        let response = await fetch(`/relay?${pin}=${status}`);
        if (response.ok) {
            let val = await response.text();
            switch (val) {
                case "0":
                    pin__text_status.textContent = 'relay status off';
                    event.target.checked = false;
                    break;
                case "1":
                    pin__text_status.textContent = 'relay status on';
                    event.target.checked = true;
                    break;
            }
        }
        else {
            switch (status) {
                case true:
                    event.target.checked = false;
                    break;
                case false:
                    event.target.checked = true;
                    break;
                default:
                    break;
            }
            pin__text_status.textContent = 'relay status error';
        }
    })();
}
if (!!window.EventSource) {
    var source = new EventSource('/events');

    source.addEventListener('change', (event) => {
        let temp__pin = JSON.parse(event.data);
        let key = Object.keys(temp__pin);
        let value = Object.values(temp__pin);
        let sw = document.getElementById(`${key[0]}`);
        let sw__text_status = document.getElementById(`${key[0]}__status`);
        let status = value[0];
        switch (status) {
            case 0:
                sw.checked = false;
                sw__text_status.textContent = 'relay status off';
                break;
            case 1:
                sw.checked = true;
                sw__text_status.textContent = 'relay status on';
                break;
            default:
                break;
        }


    }, false);
}

