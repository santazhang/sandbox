class Server

  def def func, &b
    puts "define #{func}"
    yield 3, 4
    yield 4, 5,5, 2, 3
  end

end

