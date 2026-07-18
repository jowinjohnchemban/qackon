// Qackon UI logic — no build step, no dependencies, so it works served
// straight off LittleFS with zero external requests.

const $ = (id) => document.getElementById(id);

async function fetchJSON(url, opts) {
  const res = await fetch(url, opts);
  if (!res.ok) throw new Error(`${url} -> ${res.status}`);
  return res.json();
}

function setDot(id, on) {
  $(id).classList.toggle('dot--on', on);
  $(id).classList.toggle('dot--off', !on);
}

async function pollStatus() {
  try {
    const s = await fetchJSON('/api/status');

    $('deviceName').textContent = s.deviceName;
    $('fwVersion').textContent = `version ${s.version}`;

    setDot('dotWifi', s.wifiConnected);
    setDot('dotGsm', s.gsmRegistered);
    setDot('dotGps', s.gps.valid);

    $('fixCoords').textContent = s.gps.valid
      ? `${s.gps.lat.toFixed(5)}, ${s.gps.lng.toFixed(5)}`
      : '— , —';
    $('fixMeta').textContent = s.gps.valid
      ? `${s.gps.altitudeM.toFixed(0)} m alt`
      : 'no fix';
    $('fixSpeed').textContent = s.gps.speedKmh.toFixed(1);
    $('fixSats').textContent = s.gps.satellites;
    $('imuMag').textContent = `${s.imuMagnitudeG.toFixed(2)} g`;

    const pulse = $('pulse');
    pulse.classList.toggle('is-armed', s.beaconArmed);
    $('beaconState').textContent = s.beaconArmed ? 'ARMED' : 'DISARMED';
    $('btnArm').textContent = s.beaconArmed ? 'Disarm beacon' : 'Arm beacon';
    $('btnArm').classList.toggle('is-armed', s.beaconArmed);
    $('btnArm').dataset.armed = s.beaconArmed ? '1' : '0';
  } catch (e) {
    // Device likely mid-reboot after a save/OTA — quietly retry next tick.
  }
}

async function loadConfig() {
  try {
    const c = await fetchJSON('/api/config');
    const form = $('cfgForm');
    for (const [key, val] of Object.entries(c)) {
      const el = form.elements[key];
      if (!el) continue;
      if (el.type === 'checkbox') el.checked = !!val;
      else el.value = val;
    }
  } catch (e) {
    console.error('config load failed', e);
  }
}

$('cfgForm').addEventListener('submit', async (e) => {
  e.preventDefault();
  const form = e.target;
  const data = {};
  for (const el of form.elements) {
    if (!el.name) continue;
    if (el.type === 'checkbox') data[el.name] = el.checked;
    else if (el.type === 'number') data[el.name] = parseFloat(el.value);
    else if (el.value !== '') data[el.name] = el.value; // skip blank wifiPass etc.
  }

  $('formNote').textContent = 'Saving...';
  try {
    await fetchJSON('/api/config', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data),
    });
    $('formNote').textContent = 'Saved. Reboot to apply WiFi changes.';
  } catch (e) {
    $('formNote').textContent = 'Save failed — check connection.';
  }
});

$('btnArm').addEventListener('click', async () => {
  const currentlyArmed = $('btnArm').dataset.armed === '1';
  await fetch('/api/arm', {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: `state=${currentlyArmed ? 'off' : 'on'}`,
  });
  pollStatus();
});

$('otaForm').addEventListener('submit', async (e) => {
  e.preventDefault();
  const fileInput = e.target.elements.firmware;
  if (!fileInput.files.length) return;

  const body = new FormData();
  body.append('firmware', fileInput.files[0]);

  $('otaNote').textContent = 'Uploading...';
  try {
    const res = await fetch('/api/update', { method: 'POST', body });
    if (res.ok) {
      $('otaNote').textContent = 'Update applied — device rebooting.';
    } else {
      $('otaNote').textContent = 'Update failed.';
    }
  } catch (e) {
    $('otaNote').textContent = 'Upload interrupted — device may be rebooting.';
  }
});

loadConfig();
pollStatus();
setInterval(pollStatus, 2000);
