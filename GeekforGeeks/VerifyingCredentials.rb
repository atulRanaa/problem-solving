#Verifying Credentials
#Every request sent to Twitter's API must be authorized. This exercise will have you register a new application with Twitter, obtain a consumer #key (identifies your app) and an access token (identifies a user of your app), and check to make sure that you are sending the values correctly.
#Visit https://dev.twitter.com/apps and register a new application.
#On the application settings page, click "Create my access token" to generate an access token and secret.
#Copy the values for consumer key, consumer secret, access token, and access secret into the example.

require 'rubygems'
require 'oauth'

# Change the following values to those provided on dev.twitter.com
# The consumer key identifies the application making the request.
# The access token identifies the user making the request.
consumer_key = OAuth::Consumer.new(
    "eUYIWnmgkJ8qj5GGfJenojIqR",
    "ktHMKwAjDdxsILooxrK8awdwEmgAzBRB7FWtxImwJQuspFm39P")
access_token = OAuth::Token.new(
    "3815186718-jDm0o3GzcABtJ9PJmpAcKxN661KNdwwHrXRLAlI",
    "ew62OaApFrEZJt1wZtw29XTc5QiqJFL6Z1A2xPFD47goD")

# All requests will be sent to this server.
baseurl = "https://api.twitter.com"

# The verify credentials endpoint returns a 200 status if
# the request is signed correctly.
address = URI("#{baseurl}/1.1/account/verify_credentials.json")

# Set up Net::HTTP to use SSL, which is required by Twitter.
http = Net::HTTP.new address.host, address.port
http.use_ssl = true
http.verify_mode = OpenSSL::SSL::VERIFY_PEER

# Build the request and authorize it with OAuth.
request = Net::HTTP::Get.new address.request_uri
request.oauth! http, consumer_key, access_token

# Issue the request and return the response.
http.start
response = http.request request
puts "The response status was #{response.code}"