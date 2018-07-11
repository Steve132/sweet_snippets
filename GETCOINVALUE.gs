function GETCOINVALUE(amount,currency,currencyto) {
  var url="https://api.coinmarketcap.com/v1/ticker/?limit=20&convert="+currencyto;
  var response=UrlFetchApp.fetch(url, {'muteHttpExceptions': true});
  var json = JSON.parse(response.getContentText());
  currencyto=currencyto.toLowerCase();
  for(var i=0;i<json.length;i++)
  {
    if(json[i].symbol==currency)
    {
      var price=json[i]["price_"+currencyto];
      return amount*price;
    }
  }
}
