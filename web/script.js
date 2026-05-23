async function updateValues() {
    try {
        let r = await fetch('https://192.168.1.20/gpio', { cache: 'no-store' });
        let t = await r.text();
        console.log("GPIO =", t);

        if (t == '1') {

            document.getElementById('gpio').innerHTML =
            `<div>La vanne 1 est ouverte</div>
             <div class='vert'></div>`;

        } else {

            document.getElementById('gpio').innerHTML =
            `<div>La vanne 1 est fermée</div>
             <div class='rouge'></div>`;
        }

    } catch (e) {
        console.log("Erreur GPIO:", e);
    }

    try {
        let y = await fetch('https://192.168.1.20/voltage', { cache: 'no-store' });
        let z = await y.text();
        console.log("Voltage =", z);
        let z = await y.text();

        document.getElementById('voltage').innerText =
        z + ' V';

    } catch (e) {
        console.log("Erreur voltage:", e);
    }

}

setInterval(updateValues, 10000);


const btn=document.querySelector('.btn');

btn.addEventListener('click', async()=>{
    await fetch('https://192.168.1.20/togglevanne2', { cache: 'no-store' });
});

const input=document.querySelector('#input');

input.addEventListener('input', async(event)=>{
    await fetch('https://192.168.1.20/pwm?v=' + event.target.value, { cache: 'no-store' });
});