#!/usr/bin/ruby

require 'rubygems'
require 'sinatra'

get '/' do
  "Hi!"
end

get '/hello' do
  "Hello to yourself! :D"
end

get '/email/:any' do
  "Email? " + params[:any]
end

get '/:req' do
  "I see you are seeking for " + params[:req]
end

get /.*/ do
  "What are you doing?"
end

