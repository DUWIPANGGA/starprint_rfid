const char html_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Starprint Smart Power Controller</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
        
        body {
            background-color: #f8fafc;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        
        .container {
            background: white;
            border-radius: 12px;
            box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
            width: 100%;
            max-width: 480px;
            padding: 32px;
            border: 1px solid #e5e7eb;
        }
        
        .logo {
            text-align: center;
            margin-bottom: 24px;
            padding-bottom: 16px;
            border-bottom: 1px solid #f1f5f9;
        }
        
        .logo h1 {
            color: #2563eb;
            font-size: 28px;
            font-weight: 700;
            letter-spacing: -0.5px;
        }
        
        .logo p {
            color: #64748b;
            font-size: 14px;
            margin-top: 4px;
        }
        
        .device-id {
            background: #f1f5f9;
            padding: 12px 16px;
            border-radius: 8px;
            margin: 16px 0;
            text-align: center;
            font-family: 'Courier New', monospace;
            font-weight: 600;
            color: #334155;
            border: 1px solid #e2e8f0;
            font-size: 14px;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        label {
            display: block;
            margin-bottom: 8px;
            color: #475569;
            font-weight: 500;
            font-size: 14px;
        }
        
        input[type="text"],
        input[type="password"],
        input[type="number"] {
            width: 100%;
            padding: 12px 16px;
            border: 1.5px solid #e2e8f0;
            border-radius: 8px;
            font-size: 15px;
            transition: all 0.2s;
            background: #ffffff;
            color: #334155;
        }
        
        input[type="text"]:focus,
        input[type="password"]:focus,
        input[type="number"]:focus {
            outline: none;
            border-color: #2563eb;
            box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.1);
        }
        
        .btn {
            width: 100%;
            padding: 14px;
            background: #2563eb;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 15px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s;
            margin-top: 8px;
        }
        
        .btn:hover {
            background: #1d4ed8;
        }
        
        .btn:active {
            transform: scale(0.99);
        }
        
        .btn-secondary {
            background: #64748b;
        }
        
        .btn-secondary:hover {
            background: #475569;
        }
        
        .btn-scan {
            background: #059669;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }
        
        .btn-scan:hover {
            background: #047857;
        }
        
        .btn-regenerate {
            background: #7c3aed;
        }
        
        .btn-regenerate:hover {
            background: #6d28d9;
        }
        
        .btn-info {
            background: #0ea5e9;
        }
        
        .btn-info:hover {
            background: #0284c7;
        }
        
        .message {
            padding: 12px 16px;
            border-radius: 8px;
            margin: 16px 0;
            text-align: center;
            font-size: 14px;
            display: none;
        }
        
        .success {
            background: #dcfce7;
            color: #166534;
            border: 1px solid #bbf7d0;
        }
        
        .error {
            background: #fee2e2;
            color: #991b1b;
            border: 1px solid #fecaca;
        }
        
        .networks-list {
            max-height: 220px;
            overflow-y: auto;
            border: 1.5px solid #e2e8f0;
            border-radius: 8px;
            margin-bottom: 20px;
            background: #ffffff;
        }
        
        .network-item {
            padding: 12px 16px;
            border-bottom: 1px solid #f1f5f9;
            cursor: pointer;
            transition: background 0.2s;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        
        .network-item:hover {
            background: #f8fafc;
        }
        
        .network-item:last-child {
            border-bottom: none;
        }
        
        .signal-strength {
            font-size: 12px;
            color: #64748b;
            display: flex;
            align-items: center;
            gap: 4px;
        }
        
        .status {
            text-align: center;
            margin-top: 20px;
            padding: 16px;
            border-radius: 8px;
            background: #dbeafe;
            color: #1e40af;
            border: 1px solid #bfdbfe;
            font-size: 14px;
        }
        
        .loading {
            display: inline-block;
            width: 20px;
            height: 20px;
            border: 2px solid rgba(37, 99, 235, 0.3);
            border-radius: 50%;
            border-top-color: #2563eb;
            animation: spin 1s ease-in-out infinite;
            margin-right: 10px;
        }
        
        @keyframes spin {
            to { transform: rotate(360deg); }
        }
        
        @media (max-width: 480px) {
            .container {
                padding: 24px;
            }
            
            .logo h1 {
                font-size: 24px;
            }
            
            .tabs {
                justify-content: space-between;
            }
            
            .tab {
                padding: 10px 12px;
                font-size: 13px;
                flex: 1;
                text-align: center;
            }
        }
        
        .empty-state {
            text-align: center;
            padding: 24px;
            color: #64748b;
            font-size: 14px;
        }
        
        .empty-state svg {
            margin-bottom: 12px;
            opacity: 0.7;
        }
        
        /* Navigation Tabs */
        .tabs {
            display: flex;
            border-bottom: 1px solid #e5e7eb;
            margin-bottom: 24px;
            overflow-x: auto;
            background: #f8fafc;
            border-radius: 8px;
            padding: 4px;
        }
        
        .tab {
            padding: 12px 16px;
            background: none;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            font-weight: 500;
            color: #64748b;
            cursor: pointer;
            white-space: nowrap;
            transition: all 0.2s;
            flex: 1;
            text-align: center;
        }
        
        .tab:hover {
            color: #2563eb;
            background: rgba(37, 99, 235, 0.05);
        }
        
        .tab.active {
            color: #2563eb;
            background: white;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
        }
        
        /* Page container */
        .page {
            display: none;
            animation: fadeIn 0.3s ease-in-out;
        }
        
        .page.active {
            display: block;
        }
        
        @keyframes fadeIn {
            from {
                opacity: 0;
                transform: translateY(5px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        /* Info text */
        .info-text {
            font-size: 12px;
            color: #64748b;
            margin-top: 4px;
            line-height: 1.4;
        }
        
        /* Checkbox styling */
        .checkbox-group {
            display: flex;
            align-items: center;
            margin-bottom: 16px;
        }
        
        .checkbox-group input[type="checkbox"] {
            margin-right: 8px;
            width: 18px;
            height: 18px;
            accent-color: #2563eb;
        }
        
        /* Section separator */
        .section-title {
            font-size: 16px;
            font-weight: 600;
            color: #334155;
            margin: 24px 0 16px 0;
            padding-bottom: 8px;
            border-bottom: 1px solid #e5e7eb;
        }
        
        /* Device info */
        .device-info {
            background: #f8fafc;
            border-radius: 8px;
            padding: 16px;
            margin-bottom: 20px;
            border: 1px solid #e2e8f0;
        }
        
        .info-row {
            display: flex;
            justify-content: space-between;
            margin-bottom: 8px;
            font-size: 14px;
        }
        
        .info-label {
            color: #64748b;
        }
        
        .info-value {
            color: #334155;
            font-weight: 500;
        }
        
        /* Key display */
        .key-display {
            background: #f1f5f9;
            border: 2px dashed #cbd5e1;
            border-radius: 8px;
            padding: 20px;
            text-align: center;
            margin: 20px 0;
            font-family: 'Courier New', monospace;
            font-size: 18px;
            font-weight: 600;
            color: #334155;
            letter-spacing: 2px;
            word-break: break-all;
            user-select: all;
            transition: all 0.3s;
            min-height: 80px;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        
        .key-display.visible {
            background: #dbeafe;
            border-color: #2563eb;
            box-shadow: 0 4px 12px rgba(37, 99, 235, 0.1);
        }
        
        .key-actions {
            display: flex;
            gap: 12px;
            margin-top: 20px;
        }
        
        .key-actions .btn {
            flex: 1;
        }
        
        /* Copy feedback */
        .copy-feedback {
            position: fixed;
            top: 20px;
            right: 20px;
            background: #059669;
            color: white;
            padding: 12px 20px;
            border-radius: 8px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
            display: none;
            z-index: 1000;
            animation: slideIn 0.3s ease-out;
        }
        
        @keyframes slideIn {
            from {
                transform: translateX(100%);
                opacity: 0;
            }
            to {
                transform: translateX(0);
                opacity: 1;
            }
        }
        
        /* Warning box */
        .warning-box {
            background: #fef3c7;
            border: 1px solid #fde68a;
            border-radius: 8px;
            padding: 16px;
            margin: 20px 0;
            font-size: 14px;
            color: #92400e;
            line-height: 1.5;
        }
        
        .warning-icon {
            display: inline-block;
            margin-right: 8px;
            font-size: 16px;
            vertical-align: middle;
        }
        
        /* Button group spacing */
        .button-group {
            margin-top: 24px;
        }
        
        /* Action buttons in info page */
        .action-buttons {
            display: flex;
            gap: 12px;
            margin-top: 20px;
        }
        
        .action-buttons .btn {
            flex: 1;
        }
        
        /* Footer note */
        .footer-note {
            text-align: center;
            margin-top: 24px;
            padding-top: 16px;
            border-top: 1px solid #e5e7eb;
            font-size: 12px;
            color: #94a3b8;
        }
        
        /* Page indicator */
        .page-indicator {
            text-align: center;
            font-size: 12px;
            color: #94a3b8;
            margin-bottom: 8px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">
            <h1>STARPRINT</h1>
            <p>Smart Power Controller Configuration</p>
        </div>
        
        <div class="device-id">Device ID: %DEVICE_ID%</div>
        
        <div class="tabs">
            <button class="tab active" onclick="showPage('wifi')">Wi-Fi</button>
            <button class="tab" onclick="showPage('ap')">Access Point</button>
            <button class="tab" onclick="showPage('device')">Device Key</button>
            <button class="tab" onclick="showPage('info')">Device Info</button>
        </div>
        
        <div id="message" class="message"></div>
        
        <!-- Wi-Fi Client Page -->
        <div id="wifiPage" class="page active">
            <div class="page-indicator">Wi-Fi Client Configuration</div>
            
            <button class="btn btn-scan" onclick="scanNetworks()">
                <span>🔍</span> Scan Wi-Fi Networks
            </button>
            
            <div id="networks" class="networks-list">
                <div class="empty-state">
                    <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="M12 20h9"></path>
                        <path d="M16.5 3.5a2.121 2.121 0 0 1 3 3L7 19l-4 1 1-4L16.5 3.5z"></path>
                    </svg>
                    <p>Click scan to discover available networks</p>
                </div>
            </div>
            
            <div class="form-group">
                <label for="ssid">Wi-Fi Network Name (SSID)</label>
                <input type="text" id="ssid" placeholder="Enter your Wi-Fi name" required>
                <div class="info-text">Enter the name of your wireless network</div>
            </div>
            
            <div class="form-group">
                <label for="password">Password</label>
                <input type="password" id="password" placeholder="Enter Wi-Fi password">
                <div class="info-text">Leave empty for open networks</div>
            </div>
            
            <div class="button-group">
                <button class="btn" onclick="connectWiFi()">Connect to Wi-Fi</button>
                <button class="btn btn-secondary" onclick="resetWiFi()">Reset Configuration</button>
            </div>
        </div>
        
        <!-- Access Point Page -->
        <div id="apPage" class="page">
            <div class="page-indicator">Access Point Configuration</div>
            
            <div class="section-title">Access Point Settings</div>
            
            <div class="form-group">
                <label for="apSsid">AP Name (SSID)</label>
                <input type="text" id="apSsid" placeholder="Starprint-AP">
                <div class="info-text">Name displayed when device acts as access point</div>
            </div>
            
            <div class="form-group">
                <label for="apPassword">AP Password</label>
                <input type="password" id="apPassword" placeholder="Enter AP password">
                <div class="info-text">Minimum 8 characters. Required for secure connection</div>
            </div>
            
            <div class="form-group">
                <label for="apChannel">Wi-Fi Channel</label>
                <input type="number" id="apChannel" placeholder="1-11" min="1" max="11" value="6">
                <div class="info-text">Channel for AP mode (1-11). Default is 6 for best compatibility</div>
            </div>
            
            <div class="checkbox-group">
                <input type="checkbox" id="apEnabled" checked>
                <label for="apEnabled">Enable Access Point mode when not connected to Wi-Fi</label>
            </div>
            
            <div class="info-text">
                When enabled, the device will create its own Wi-Fi network if it cannot connect to your home Wi-Fi.
            </div>
            
            <div class="button-group">
                <button class="btn" onclick="saveApSettings()">Save AP Settings</button>
            </div>
        </div>
        
        <!-- Device Key Page -->
        <div id="devicePage" class="page">
            <div class="page-indicator">Device Authentication</div>
            
            <div class="section-title">Authentication Key</div>
            
            <div class="device-info">
                <div class="info-row">
                    <span class="info-label">Device ID:</span>
                    <span class="info-value">%DEVICE_ID%</span>
                </div>
                <div class="info-row">
                    <span class="info-label">Current Key:</span>
                    <span class="info-value" id="currentKeyStatus">••••••••</span>
                </div>
                <div class="info-row">
                    <span class="info-label">Key Age:</span>
                    <span class="info-value" id="keyAge">0 days</span>
                </div>
            </div>
            
            <div class="warning-box">
                <span class="warning-icon">⚠️</span>
                <strong>Important:</strong> This key is used for device authentication.
            </div>
            
            <div id="keyDisplay" class="key-display">
                ••••••••••••••••
            </div>
            
            <div class="key-actions">
                <button class="btn btn-regenerate" onclick="regenerateKey()">
                    <span>🔄</span> Regenerate Key
                </button>
                <button class="btn btn-secondary" onclick="toggleKeyVisibility()">
                    <span id="showHideIcon">👁️</span> <span id="showHideText">Show Key</span>
                </button>
            </div>
            
            <div class="info-text" style="text-align: center; margin-top: 12px;">
                Click "Show Key" to reveal, then click on the key to copy it to clipboard
            </div>
        </div>
        
        <!-- Device Info Page -->
        <div id="infoPage" class="page">
            <div class="page-indicator">Device Status & Information</div>
            
            <div class="section-title">Device Information</div>
            
            <div class="device-info">
                <div class="info-row">
                    <span class="info-label">Device ID:</span>
                    <span class="info-value" id="infoDeviceId">%DEVICE_ID%</span>
                </div>
                <div class="info-row">
                    <span class="info-label">Firmware Version:</span>
                    <span class="info-value" id="infoFirmware">v1.0.0</span>
                </div>
                <div class="info-row">
                    <span class="info-label">Wi-Fi Status:</span>
                    <span class="info-value" id="infoWifiStatus">Disconnected</span>
                </div>
                <div class="info-row">
                    <span class="info-label">IP Address:</span>
                    <span class="info-value" id="infoIpAddress">192.168.4.1</span>
                </div>
                <div class="info-row">
                    <span class="info-label">AP Status:</span>
                    <span class="info-value" id="infoApStatus">Active</span>
                </div>
                <div class="info-row">
                    <span class="info-label">Uptime:</span>
                    <span class="info-value" id="infoUptime">0h 0m</span>
                </div>
                <div class="info-row">
                    <span class="info-label">Free Memory:</span>
                    <span class="info-value" id="infoMemory">128KB free</span>
                </div>
            </div>
            
            <div class="section-title">Device Actions</div>
            
            <div class="action-buttons">
                <button class="btn btn-info" onclick="loadDeviceInfo()">
                    Refresh Info
                </button>
                <button class="btn btn-secondary" onclick="restartDevice()">
                     Restart Device
                </button>
            </div>
            
            <div class="info-text" style="margin-top: 16px;">
                <strong>Note:</strong> Restarting the device will temporarily disconnect all connections. It will reconnect automatically.
            </div>
        </div>
        
        <div id="status" class="status" style="display: none;">
            <div class="loading"></div>
            <span id="statusText">Connecting... Please wait.</span>
        </div>
        
        <div class="footer-note">
            Starprint Smart Power Controller © 2024
        </div>
    </div>
    
    <!-- Copy Feedback -->
    <div id="copyFeedback" class="copy-feedback">
        ✅ Key copied to clipboard!
    </div>
<script>
    let deviceKey = '';
    let isKeyVisible = false;
    let keyGeneratedDate = new Date();

    // Fungsi utama untuk memuat data awal
    async function loadInitialData() {
        try {
            const res = await fetch('/api/detail');
            const data = await res.json();
            
            console.log('Device detail loaded:', data);

            /* ================= UPDATE SEMUA VARIABLE ================= */
            
            // 1. Update Device ID di semua tempat
            document.querySelectorAll('.device-id').forEach(el => {
                el.textContent = 'Device ID: ' + data.deviceId;
            });
            
            // 2. Device Key
            if (data.deviceKey) {
                deviceKey = data.deviceKey;
                keyGeneratedDate = new Date(); // Anda bisa simpan timestamp di config
                updateKeyDisplay();
                document.getElementById('currentKeyStatus').textContent = 'Active';
            } else {
                document.getElementById('currentKeyStatus').textContent = 'Not Set';
            }
            
            // 3. WiFi Form
            if (data.wifi_ssid) {
                document.getElementById('ssid').value = data.wifi_ssid;
            }
            if (data.wifi_pass) {
                document.getElementById('password').value = data.wifi_pass;
            }
            
            // 4. AP Form
            if (data.ap_name) {
                document.getElementById('apSsid').value = data.ap_name;
            }
            
            // Password AP (tidak ditampilkan untuk keamanan, tapi bisa diisi kosong)
            document.getElementById('apPassword').value = '';
            
            // Channel AP
            if (data.ap_channel) {
                document.getElementById('apChannel').value = data.ap_channel;
            }
            
            // Status AP enabled
            document.getElementById('apEnabled').checked = data.apStatus === 'Active' && data.ap_enabled !== false;
            
            /* ================= DEVICE INFO PAGE ================= */
            document.getElementById('infoDeviceId').textContent = data.deviceId;
            document.getElementById('infoFirmware').textContent = 'v' + data.firmware;
            document.getElementById('infoWifiStatus').textContent = data.wifiStatus;
            document.getElementById('infoIpAddress').textContent = data.ipAddress;
            document.getElementById('infoApStatus').textContent = data.apStatus;
            document.getElementById('infoMemory').textContent = data.memory;
            
            // Format uptime (detik → jam:menit)
            const h = Math.floor(data.uptime / 3600);
            const m = Math.floor((data.uptime % 3600) / 60);
            document.getElementById('infoUptime').textContent = `${h}h ${m}m`;
            
            // Key age (gunakan keyAge dari server jika ada)
            if (data.keyAge) {
                document.getElementById('keyAge').textContent = data.keyAge;
            } else {
                document.getElementById('keyAge').textContent = calculateKeyAge();
            }
            
        } catch (err) {
            console.error('Failed to load device data:', err);
            showMessage('Failed to load device data. Please refresh.', 'error');
        }
    }

    // Fungsi untuk memuat ulang data perangkat (info page)
    async function loadDeviceInfo() {
        try {
            showStatus('Loading device information...');
            
            const res = await fetch('/api/detail');
            const data = await res.json();
            
            document.getElementById('infoDeviceId').textContent = data.deviceId;
            document.getElementById('infoFirmware').textContent = 'v' + data.firmware;
            document.getElementById('infoWifiStatus').textContent = data.wifiStatus;
            document.getElementById('infoIpAddress').textContent = data.ipAddress;
            document.getElementById('infoApStatus').textContent = data.apStatus;
            document.getElementById('infoMemory').textContent = data.memory;
            
            // Format uptime
            const h = Math.floor(data.uptime / 3600);
            const m = Math.floor((data.uptime % 3600) / 60);
            document.getElementById('infoUptime').textContent = `${h}h ${m}m`;
            
            hideStatus();
            showMessage('Device information refreshed', 'success');
            
        } catch (error) {
            console.error('Error loading device info:', error);
            showMessage('Failed to refresh device info', 'error');
            hideStatus();
        }
    }

    // Generate secure random key (fallback jika API tidak ada)
    function generateSecureKey() {
        const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*';
        let key = '';
        for (let i = 0; i < 24; i++) {
            key += chars.charAt(Math.floor(Math.random() * chars.length));
        }
        return key;
    }
    
    // Calculate key age
    function calculateKeyAge() {
        const now = new Date();
        const diffTime = Math.abs(now - keyGeneratedDate);
        const diffDays = Math.floor(diffTime / (1000 * 60 * 60 * 24));
        if (diffDays === 0) return 'Today';
        if (diffDays === 1) return '1 day ago';
        return `${diffDays} days ago`;
    }
    
    // Show/hide page functions
    function showPage(pageName) {
        // Hide all pages
        document.querySelectorAll('.page').forEach(page => {
            page.classList.remove('active');
        });
        
        // Remove active class from all tabs
        document.querySelectorAll('.tab').forEach(tab => {
            tab.classList.remove('active');
        });
        
        // Show selected page
        document.getElementById(pageName + 'Page').classList.add('active');
        
        // Set active tab
        event.target.classList.add('active');
        
        // Update key display when showing device page
        if (pageName === 'device') {
            updateKeyDisplay();
            document.getElementById('keyAge').textContent = calculateKeyAge();
        }
        
        // Load info when showing info page
        if (pageName === 'info') {
            loadDeviceInfo();
        }
    }
    
    // Update key display
    function updateKeyDisplay() {
        const keyDisplay = document.getElementById('keyDisplay');
        const showHideIcon = document.getElementById('showHideIcon');
        const showHideText = document.getElementById('showHideText');
        
        if (!deviceKey) {
            keyDisplay.textContent = 'No key generated yet';
            keyDisplay.classList.remove('visible');
            return;
        }
        
        if (isKeyVisible) {
            keyDisplay.textContent = deviceKey;
            keyDisplay.classList.add('visible');
            keyDisplay.style.cursor = 'pointer';
            keyDisplay.title = 'Click to copy key to clipboard';
            keyDisplay.onclick = copyKeyToClipboard;
            showHideIcon.textContent = '🙈';
            showHideText.textContent = 'Hide Key';
        } else {
            keyDisplay.textContent = '•'.repeat(deviceKey.length || 16);
            keyDisplay.classList.remove('visible');
            keyDisplay.style.cursor = 'default';
            keyDisplay.title = '';
            keyDisplay.onclick = null;
            showHideIcon.textContent = '👁️';
            showHideText.textContent = 'Show Key';
        }
    }
    
    // Toggle key visibility
    function toggleKeyVisibility() {
        if (!deviceKey) {
            showMessage('No device key available. Please generate one first.', 'error');
            return;
        }
        
        isKeyVisible = !isKeyVisible;
        updateKeyDisplay();
        
        if (isKeyVisible) {
            showCopyInstruction();
        }
    }
    
    // Show copy instruction
    function showCopyInstruction() {
        const messageDiv = document.getElementById('message');
        messageDiv.textContent = 'Key is now visible. Click on the key to copy it to clipboard.';
        messageDiv.className = 'message success';
        messageDiv.style.display = 'block';
        
        setTimeout(() => {
            messageDiv.style.display = 'none';
        }, 3000);
    }
    
    // Copy key to clipboard
    function copyKeyToClipboard() {
        if (!isKeyVisible || !deviceKey) return;
        
        navigator.clipboard.writeText(deviceKey).then(() => {
            // Show feedback
            const feedback = document.getElementById('copyFeedback');
            feedback.style.display = 'block';
            
            setTimeout(() => {
                feedback.style.display = 'none';
            }, 2000);
        }).catch(err => {
            console.error('Failed to copy: ', err);
            showMessage('Failed to copy key. Please select and copy manually.', 'error');
        });
    }
    
    // Regenerate key (menggunakan API)
    async function regenerateKey() {
        if (!confirm('Generate new device key? The old key will become invalid immediately.')) {
            return;
        }
        
        showStatus('Generating new key...');
        
        try {
            const response = await fetch('/api/key/regenerate', {
                method: 'POST'
            });
            
            const data = await response.json();
            
            if (data.success) {
                deviceKey = data.key;
                keyGeneratedDate = new Date();
                isKeyVisible = true; // Show new key automatically
                
                updateKeyDisplay();
                document.getElementById('keyAge').textContent = calculateKeyAge();
                document.getElementById('currentKeyStatus').textContent = 'Active';
                
                hideStatus();
                showMessage('New key generated successfully. Click on the key to copy it.', 'success');
                
                // Auto-hide after 30 seconds
                setTimeout(() => {
                    if (isKeyVisible) {
                        isKeyVisible = false;
                        updateKeyDisplay();
                    }
                }, 30000);
            } else {
                hideStatus();
                showMessage('Failed to generate key: ' + data.message, 'error');
            }
        } catch (error) {
            hideStatus();
            showMessage('Error regenerating key. Please try again.', 'error');
        }
    }
    
    // Original Wi-Fi functions (dengan API asli)
    function scanNetworks() {
        const networksDiv = document.getElementById('networks');
        networksDiv.innerHTML = '<div class="empty-state"><div class="loading"></div><p>Scanning for networks...</p></div>';
        
        fetch('/api/scan')
            .then(response => response.json())
            .then(data => {
                networksDiv.innerHTML = '';
                
                if (data.networks && data.networks.length > 0) {
                    data.networks.forEach(network => {
                        const signalBars = Math.min(
                            5,
                            Math.max(1, Math.ceil((100 + network.rssi) / 20))
                        );

                        const item = document.createElement('div');
                        item.className = 'network-item';
                        item.innerHTML = `
                            <div><strong>${network.ssid}</strong></div>
                            <div class="signal-strength">
                                ${'●'.repeat(signalBars)}${'○'.repeat(5-signalBars)}
                                ${network.encryption ? '🔒' : '🌐'}
                            </div>
                        `;
                        item.onclick = () => {
                            document.getElementById('ssid').value = network.ssid;
                            document.getElementById('password').focus();
                        };
                        networksDiv.appendChild(item);
                    });
                } else {
                    networksDiv.innerHTML = '<div class="empty-state"><p>No Wi-Fi networks found. Please try again.</p></div>';
                }
            })
            .catch(error => {
                showMessage('Error scanning networks. Please try again.', 'error');
                networksDiv.innerHTML = '<div class="empty-state"><p>Scan failed. Try again.</p></div>';
            });
    }
    
    function connectWiFi() {
        const ssid = document.getElementById('ssid').value;
        const password = document.getElementById('password').value;
        
        if (!ssid) {
            showMessage('Please enter Wi-Fi network name', 'error');
            return;
        }
        
        // Hide form and show status
        document.getElementById('wifiPage').style.display = 'none';
        document.getElementById('status').style.display = 'block';
        document.getElementById('statusText').textContent = 'Connecting to Wi-Fi...';
        
        fetch('/api/connect', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                ssid: ssid,
                password: password
            })
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                document.getElementById('statusText').textContent = 'Connected successfully! Restarting device...';
                setTimeout(() => {
                    window.location.href = '/api/status';
                }, 3000);
            } else {
                showMessage('Connection failed: ' + data.message, 'error');
                document.getElementById('wifiPage').style.display = 'block';
                document.getElementById('status').style.display = 'none';
            }
        })
        .catch(error => {
            showMessage('Connection error. Please check your network.', 'error');
            document.getElementById('wifiPage').style.display = 'block';
            document.getElementById('status').style.display = 'none';
        });
    }
    
    function resetWiFi() {
        if (confirm('Reset Wi-Fi configuration? Device will restart in setup mode.')) {
            fetch('/api/reset', { method: 'POST' })
                .then(() => {
                    showMessage('Reset successful. Device restarting...', 'success');
                    setTimeout(() => {
                        window.location.reload();
                    }, 2000);
                })
                .catch(error => {
                    showMessage('Reset failed. Please try again.', 'error');
                });
        }
    }
    
    // AP Settings functions (dengan API asli)
    async function saveApSettings() {
        const apData = {
            ssid: document.getElementById('apSsid').value,
            password: document.getElementById('apPassword').value,
            channel: parseInt(document.getElementById('apChannel').value),
            enabled: document.getElementById('apEnabled').checked
        };
        
        if (!apData.ssid) {
            showMessage('Please enter AP name', 'error');
            return;
        }
        
        if (apData.password && apData.password.length < 8) {
            showMessage('AP password must be at least 8 characters', 'error');
            return;
        }
        
        showStatus('Saving AP settings...');
        
        try {
            const response = await fetch('/api/ap/settings', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(apData)
            });
            
            const data = await response.json();
            
            if (data.success) {
                showMessage('AP settings saved successfully', 'success');
            } else {
                showMessage('Failed to save AP settings: ' + data.message, 'error');
            }
        } catch (error) {
            showMessage('Error saving AP settings', 'error');
        } finally {
            hideStatus();
        }
    }
    
    async function restartDevice() {
        if (confirm('Restart device? This will temporarily disconnect all connections.')) {
            showStatus('Restarting device...');
            
            try {
                const response = await fetch('/api/restart', {
                    method: 'POST'
                });
                
                showMessage('Device is restarting... Please wait', 'success');
                setTimeout(() => {
                    window.location.reload();
                }, 3000);
            } catch (error) {
                showMessage('Error restarting device', 'error');
                hideStatus();
            }
        }
    }
    
    // Utility functions
    function showMessage(text, type) {
        const msgDiv = document.getElementById('message');
        msgDiv.textContent = text;
        msgDiv.className = 'message ' + type;
        msgDiv.style.display = 'block';
        
        setTimeout(() => {
            msgDiv.style.display = 'none';
        }, 5000);
    }
    
    function showStatus(text) {
        document.getElementById('statusText').textContent = text;
        document.getElementById('status').style.display = 'block';
    }
    
    function hideStatus() {
        document.getElementById('status').style.display = 'none';
    }
    
    // Initialize saat halaman dimuat
    window.onload = function() {
        // 1. Load semua data dari API
        loadInitialData();
        
        // 2. Auto-scan networks pada halaman WiFi
        scanNetworks();
        
        // 3. Set default AP name jika kosong
        const apSsidInput = document.getElementById('apSsid');
        if (!apSsidInput.value) {
            // Ambil 4 karakter terakhir dari device ID
            const deviceIdElement = document.querySelector('.device-id');
            const deviceId = deviceIdElement.textContent.split(': ')[1] || 'STAR';
            const lastFour = deviceId.slice(-4);
            apSsidInput.value = 'Starprint-' + lastFour;
        }
    };
</script>
</body>
</html>
)=====";