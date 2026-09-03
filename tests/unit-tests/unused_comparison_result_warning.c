void unused_comparison_result_warning(int left_value, int right_value)
{
    //warning: expression result not used
    left_value == right_value; //lint 10

    //warning: expression result not used
    left_value != right_value; //lint 10

    //warning: expression result not used
    left_value < right_value; //lint 10

    //warning: expression result not used
    left_value > right_value; //lint 10

    //warning: expression result not used
    left_value <= right_value; //lint 10

    //warning: expression result not used
    left_value >= right_value; //lint 10
}
