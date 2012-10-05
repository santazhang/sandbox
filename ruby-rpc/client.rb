class Client

  def method_missing m, *a
    puts "method_missing: #{m} #{a} #{a.class}"
    if m.to_s.start_with? "async_"
      puts "async: #{m}"
      return Future.new
    elsif m.to_s.start_with? "sync_"
      puts "sync: #{m}"
      return "BLAH"
    else
      super
    end
  end

end

class Future
  def get
    return "TODO"
  end
end
