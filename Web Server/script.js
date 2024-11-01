const brokerUrl = "wss://broker.hivemq.com:8884/mqtt";
const topic = "peter/parkdui";
let count = 4;

const client = mqtt.connect(brokerUrl);

client.on("connect", () => {
    console.log("Connected to MQTT Broker");
    client.subscribe(topic, (err) => {
        if (!err) {
            console.log(`Subscribed to topic: ${topic}`);
        } else {
            console.error("Failed to subscribe:", err);
        }
    });
});

function updateCounterColor() {
    const counterElement = document.getElementById("counter");
    counterElement.textContent = count;

    let green = Math.max(0, 255 - Math.abs(count - 4) * 40);
    let red = Math.min(255, Math.abs(count - 4) * 40);

    counterElement.style.backgroundColor = `rgb(${red}, ${green}, 0)`;
}

client.on("message", (topic, message) => {
    const mqttDataList = document.getElementById("mqtt-data-list");
    const dataItem = document.createElement("div");
    const timestamp = new Date().toLocaleTimeString();
    const msg = message.toString().trim().toUpperCase();

    if (msg === "IN") {
        dataItem.classList.add("data-item", "in");
        dataItem.textContent = `[${timestamp}] IN`;
        count--;
    } else if (msg === "OUT") {
        dataItem.classList.add("data-item", "out");
        dataItem.textContent = `[${timestamp}] OUT`;
        count++;
    } else {
        return;
    }

    updateCounterColor();
    mqttDataList.prepend(dataItem);
    console.log(`Received message on ${topic}: ${message}`);
});