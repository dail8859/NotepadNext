import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import yfinance as yf

# Liste des cryptomonnaies du BCI5 et leurs tickers
cryptos = {
    "Bitcoin": "BTC-USD",
    "Ethereum": "ETH-USD",
    "Cardano": "ADA-USD",
    "XRP": "XRP-USD",
    "Polkadot": "DOT-USD"
}

# Periode des donnees (5 ans)
start_date = "2018-01-01"
end_date = "2023-12-31"

# Recuperation des donnees pour chaque crypto
data = {}
for name, ticker in cryptos.items():
    print(f"Telechargement des donnees pour {name}...")
    data[name] = yf.download(ticker, start=start_date, end=end_date)["Close"]

# Organisation des donnees dans un DataFrame
df = pd.DataFrame(data)

# Creation d'un Rainbow Chart pour chaque crypto
for crypto in df.columns:
    plt.figure(figsize=(10, 6))
    prices = df[crypto].dropna()
    log_prices = np.log(prices)

    # Definir les bandes de couleurs (zones)
    bands = [0.5, 1, 2, 4, 8, 16, 32, 64]
    colors = ['purple', 'blue', 'green', 'yellow', 'orange', 'red']

    for i in range(len(bands) - 1):
        plt.fill_between(
            prices.index,
            np.exp(log_prices.min()) * bands[i],
            np.exp(log_prices.min()) * bands[i + 1],
            color=colors[i % len(colors)],
            alpha=0.2,
        )

    plt.plot(prices.index, prices, label=crypto, color='black', linewidth=2)
    plt.yscale("log")
    plt.title(f"Rainbow Chart for {crypto}")
    plt.xlabel("Date")
    plt.ylabel("Price (Log Scale)")
    plt.legend()
    plt.grid()
    plt.show()
