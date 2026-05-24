let busy = false;

const TOKEN = "2755";

async function updateValues() {
    if (busy) return;

    busy = true;

    try {
        let r = await fetch('https://192.168.1.20/status?token=' + TOKEN, {
            cache: 'no-store'
        });

        let data = await r.json();

        if (data.gpio == 1) {
            document.getElementById('gpio').innerHTML =
                `<div>La vanne 1 est ouverte</div><div class='vert'></div>`;
        } else {
            document.getElementById('gpio').innerHTML =
                `<div>La vanne 1 est fermée</div><div class='rouge'></div>`;
        }

        document.getElementById('voltage').innerText =
            data.voltage + ' V';

    } catch (e) {
        console.log("Erreur updateValues:", e.message);

    } finally {
        busy = false;
    }
}

updateValues();

setInterval(() => {
    updateValues();
}, 5000);

const btn = document.querySelector('.btn');

btn.addEventListener('click', async () => {
    try {
        await fetch('https://192.168.1.20/togglevanne2?token=' + TOKEN, {
            cache: 'no-store'
        });

        updateValues();

    } catch (e) {
        console.log("Erreur button:", e.message);
    }
});

const input = document.querySelector('#input');

input.addEventListener('change', async (event) => {
    try {
        await fetch('https://192.168.1.20/pwm?v=' + event.target.value + '&token=' + TOKEN, {
            cache: 'no-store'
        });

    } catch (e) {
        console.log("Erreur PWM:", e.message);
    }
});