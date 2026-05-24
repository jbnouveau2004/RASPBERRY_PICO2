let busy = false;

async function updateValues() {
    if (busy) return;

    busy = true;

    try {
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

        document.getElementById('voltage').innerText = z + ' V';

    } catch (e) {
        console.log("Erreur updateValues:", e.message);

    } finally {
        busy = false;
    }
}

updateValues();
setInterval(() => {
    updateValues();
}, 3000);

const btn=document.querySelector('.btn');
btn.addEventListener('click', async() => {
    try {
        await fetch('https://192.168.1.20/togglevanne2', {
            cache: 'no-store'
        });
    } catch (e) {
        console.log("Erreur button:", e.message);
    }
});


input.addEventListener('input', async(event) => {
    try {
        await fetch('https://192.168.1.20/pwm?v=' + event.target.value, {
            cache: 'no-store'
        });
    } catch (e) {
        console.log("Erreur PWM:", e.message);
    }
});