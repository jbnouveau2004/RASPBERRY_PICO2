let busy = false;

async function updateValues() {
    busy = true;

    let r = await fetch('https://192.168.1.20/gpio', { cache: 'no-store' });
        let t = await r.text();

        if (t == '1') {
            document.getElementById('gpio').innerHTML =
            `<div>La vanne 1 est ouverte</div><div class='vert'></div>`;
        } else {
            document.getElementById('gpio').innerHTML =
            `<div>La vanne 1 est fermée</div><div class='rouge'></div>`;
        }

        let y = await fetch('https://192.168.1.20/voltage', { cache: 'no-store' });
        let z = await y.text();
        document.getElementById('voltage').innerText =
        z + ' V';

    busy = false;  
}

updateValues();
setInterval("if(!busy){updateValues();}", 1000);


const btn=document.querySelector('.btn');

btn.addEventListener('click', async()=>{
    await fetch('https://192.168.1.20/togglevanne2', { cache: 'no-store' });
});

const input=document.querySelector('#input');

input.addEventListener('input', async(event)=>{
    await fetch('https://192.168.1.20/pwm?v=' + event.target.value, { cache: 'no-store' });
});