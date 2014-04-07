require 'test_helper'

class RunsControllerTest < ActionController::TestCase
  # test "the truth" do
  #   assert true
  # 
  def setup

  end

  test "make new run" do
  	post :new, post: {hr: "1", min: "1", sec:"1", temp: "1", humidity:"1", name:"test", dist:"1", date:"1/1/2104"}

  	assert_redirected_to post_path(assigns(:post))
  end
end
