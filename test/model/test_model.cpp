/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <cppmh.h>

/*****************************************************************************/
namespace {
class TestModel : public ::testing::Test {
   protected:
    cppmh::utility::IntegerUniformRandom m_random_integer;
    cppmh::utility::IntegerUniformRandom m_random_positive_integer;

    virtual void SetUp(void) {
        m_random_integer.setup(-1000, 1000, 0);
        m_random_positive_integer.setup(1, 1000, 0);
    }
    virtual void TearDown() {
        /// nothing to do
    }
    int random_integer(void) {
        return m_random_integer.generate_random();
    }

    int random_positive_integer(void) {
        return m_random_positive_integer.generate_random();
    }
};

/*****************************************************************************/
TEST_F(TestModel, initialize) {
    cppmh::model::Model<int, double> model;

    /// These declarations are for googletest constraint.
    /// http://opencv.jp/googletestdocs/FAQ.html#faq-the-compiler-complains-about-undefined-reference
    auto max_number_of_variable_proxies =
        cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
    auto max_number_of_expression_proxies =
        cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
    auto max_number_of_constraint_proxies =
        cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;

    EXPECT_EQ(max_number_of_variable_proxies,
              static_cast<int>(model.variable_proxies().capacity()));
    EXPECT_EQ(max_number_of_expression_proxies,
              static_cast<int>(model.expression_proxies().capacity()));
    EXPECT_EQ(max_number_of_constraint_proxies,
              static_cast<int>(model.constraint_proxies().capacity()));

    EXPECT_EQ(true, model.variable_names().empty());
    EXPECT_EQ(true, model.expression_names().empty());
    EXPECT_EQ(true, model.constraint_names().empty());

    EXPECT_EQ(false, model.is_defined_objective());
    EXPECT_EQ(true, model.is_enabled_fast_evaluation());
    EXPECT_EQ(true, model.is_linear());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());

    /// Variable Reference
    EXPECT_EQ(  //
        true, model.variable_reference().variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().fixed_variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().selection_variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().binary_variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().integer_variable_ptrs.empty());
    EXPECT_EQ(  //
        true,   //
        model.variable_reference().variable_ptrs.empty());

    /// Constraint Reference
    EXPECT_EQ(  //
        true,   //
        model.constraint_reference().constraint_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_reference().selection_constraint_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_reference().disabled_constraint_ptrs.empty());

    /// Constraint Type Reference
    EXPECT_EQ(  //
        true, model.constraint_type_reference().singleton_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().aggregation_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().precedence_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().variable_bound_ptrs.empty());
    EXPECT_EQ(  //
        true,

        model.constraint_type_reference().set_partitioning_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().set_packing_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().set_covering_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().cardinality_ptrs.empty());
    EXPECT_EQ(  //
        true,
        model.constraint_type_reference().invariant_knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().equation_knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().bin_packing_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().integer_knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().general_linear_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().nonlinear_ptrs.empty());
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_scalar_without_bound) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.id());
            EXPECT_EQ(std::numeric_limits<int>::min() + 1, x.lower_bound());
            EXPECT_EQ(std::numeric_limits<int>::max() - 1, x.upper_bound());
            EXPECT_EQ(false, x.has_bounds());
            EXPECT_EQ(cppmh::model::VariableSense::Integer, x.sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variable("error"), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_variable("s p a c e"), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_scalar_with_bound) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.id());
            EXPECT_EQ(0, x.lower_bound());
            EXPECT_EQ(1, x.upper_bound());
            EXPECT_EQ(true, x.has_bounds());
            EXPECT_EQ(cppmh::model::VariableSense::Binary, x.sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variable("error", 0, 1), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_variable("s p a c e", 0, 1),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_one_dimensional_without_bound) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.id());
            EXPECT_EQ(std::numeric_limits<int>::min() + 1, x(0).lower_bound());
            EXPECT_EQ(std::numeric_limits<int>::max() - 1, x(0).upper_bound());
            EXPECT_EQ(false, x(0).has_bounds());
            EXPECT_EQ(cppmh::model::VariableSense::Integer, x(0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", 2), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", 2), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_one_dimensional_with_bound) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.id());
            EXPECT_EQ(0, x(0).lower_bound());
            EXPECT_EQ(1, x(0).upper_bound());
            EXPECT_EQ(true, x(0).has_bounds());
            EXPECT_EQ(cppmh::model::VariableSense::Binary, x(0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", 2, 0, 1),
                     std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", 2, 0, 1),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_two_dimensional_without_bound) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3});
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.id());
            EXPECT_EQ(std::numeric_limits<int>::min() + 1,
                      x(0, 0).lower_bound());
            EXPECT_EQ(std::numeric_limits<int>::max() - 1,
                      x(0, 0).upper_bound());
            EXPECT_EQ(false, x(0, 0).has_bounds());
            EXPECT_EQ(cppmh::model::VariableSense::Integer, x(0, 0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", {2, 3}), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", {2, 3}),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_two_dimensional_with_bound) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3}, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.id());
            EXPECT_EQ(0, x(0, 0).lower_bound());
            EXPECT_EQ(1, x(0, 0).upper_bound());
            EXPECT_EQ(true, x(0, 0).has_bounds());
            EXPECT_EQ(cppmh::model::VariableSense::Binary, x(0, 0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", {2, 3}, 0, 1),
                     std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", {2, 3}, 0, 1),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_scalar) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expression(name);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.id());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expression("error"), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_expression("s p a c e"), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_one_dimensional) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expressions(name, 2);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.id());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expressions("error", 2), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_expressions("s p a c e", 2),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_two_dimensional) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expressions(name, {2, 3});
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.id());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expressions("error", {2, 3}),
                     std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_expressions("s p a c e", {2, 3}),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_arg_expression_like) {
    {
        cppmh::model::Model<int, double> model;

        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto name = "p" + std::to_string(i);
            auto variable =
                cppmh::model::Variable<int, double>::create_instance();

            auto& p = model.create_expression(name, variable);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.id());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }

        auto variable = cppmh::model::Variable<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("error", variable),
                     std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        auto variable = cppmh::model::Variable<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("s p a c e", variable),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_arg_expression) {
    {
        cppmh::model::Model<int, double> model;

        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto name = "p" + std::to_string(i);
            auto expression =
                cppmh::model::Expression<int, double>::create_instance();

            auto& p = model.create_expression(name, expression);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.id());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }

        auto expression =
            cppmh::model::Expression<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("error", expression),
                     std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;

        auto expression =
            cppmh::model::Expression<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("s p a c e", expression),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_scalar) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraint(name);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.id());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraint("error"), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_constraint("s p a c e"), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_one_dimensional) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraints(name, 2);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.id());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraints("error", 2), std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_constraints("s p a c e", 2),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_two_dimensional) {
    {
        cppmh::model::Model<int, double> model;
        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraints(name, {2, 3});
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.id());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraints("error", {2, 3}),
                     std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.create_constraints("s p a c e", {2, 3}),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_arg_constraint) {
    {
        cppmh::model::Model<int, double> model;

        for (auto i = 0;
             i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto name = "g" + std::to_string(i);
            auto expression =
                cppmh::model::Expression<int, double>::create_instance();
            auto constraint = expression <= 1;

            auto& g = model.create_constraint(name, constraint);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.id());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }

        auto expression =
            cppmh::model::Expression<int, double>::create_instance();
        auto constraint = expression <= 1;
        ASSERT_THROW(model.create_constraint("error", constraint),
                     std::logic_error);
    }
    {
        cppmh::model::Model<int, double> model;
        auto                             expression =
            cppmh::model::Expression<int, double>::create_instance();
        auto constraint = expression <= 1;
        ASSERT_THROW(model.create_constraint("s p a c e", constraint),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_function) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    auto f = [&p](const cppmh::model::Move<int, double>& a_MOVE) {
        return p.evaluate(a_MOVE);
    };
    model.minimize(f);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(true, model.objective().expression().sensitivities().empty());
    EXPECT_EQ(0, model.objective().expression().constant_value());
    EXPECT_EQ(false, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_expression_like) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const cppmh::model::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.minimize(p);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_expression) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const cppmh::model::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.minimize(p(0));

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_function) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    auto f = [&p](const cppmh::model::Move<int, double>& a_MOVE) {
        return p.evaluate(a_MOVE);
    };
    model.maximize(f);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(true, model.objective().expression().sensitivities().empty());
    EXPECT_EQ(0, model.objective().expression().constant_value());
    EXPECT_EQ(false, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression_like) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const cppmh::model::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.maximize(p);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression) {
    cppmh::model::Model<int, double> model;
    auto&                            x = model.create_variables("x", 10, -1, 1);
    auto&                 p = model.create_expression("p", x.sum() + 1);
    [[maybe_unused]] auto f =
        [&p](const cppmh::model::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.maximize(p(0));

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, setup) {
    /// This method is tested in the following submethods.
}

/*****************************************************************************/
TEST_F(TestModel, verify_problem) {
    /// No decision variables.
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.verify_problem(false), std::logic_error);
    }

    /// No constraint functions.
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");
        model.minimize(x);
        model.verify_problem(false);
    }

    /// No objective function.
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");
        model.create_constraint("g", x == 1);
        model.verify_problem(false);
    }

    /// No constraint functions and no objective function
    {
        cppmh::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variable("x");
        ASSERT_THROW(model.verify_problem(false), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_related_constraints) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);

    auto& g = model.create_constraints("g", 3);
    g(0)    = x.selection();
    g(1)    = y.selection();
    g(2)    = x(0) + y.sum({0, cppmh::model::All}) >= 1;

    model.setup_variable_related_constraints();

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(true, x(i).related_constraint_ptrs().find(&g(0)) !=
                            x(i).related_constraint_ptrs().end());
        EXPECT_EQ(false, x(i).related_constraint_ptrs().find(&g(1)) !=
                             x(i).related_constraint_ptrs().end());
        /// Only x(0) is related to g(2).
        EXPECT_EQ(i == 0, x(i).related_constraint_ptrs().find(&g(2)) !=
                              x(i).related_constraint_ptrs().end());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(false, y(i, j).related_constraint_ptrs().find(&g(0)) !=
                                 y(i, j).related_constraint_ptrs().end());
            EXPECT_EQ(true, y(i, j).related_constraint_ptrs().find(&g(1)) !=
                                y(i, j).related_constraint_ptrs().end());
            /// Only y(0,*) is related to g(2).
            EXPECT_EQ(i == 0, y(i, j).related_constraint_ptrs().find(&g(2)) !=
                                  y(i, j).related_constraint_ptrs().end());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_sense) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    model.create_constraint("g", x.selection());

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(cppmh::model::SelectionMode::Defined);
    for (const auto& element : x.flat_indexed_variables()) {
        EXPECT_EQ(cppmh::model::VariableSense::Selection, element.sense());
    }

    model.setup_variable_sense();
    for (const auto& element : x.flat_indexed_variables()) {
        EXPECT_EQ(cppmh::model::VariableSense::Binary, element.sense());
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_unique_name) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& p = model.create_variables("p", {10});
    auto& g = model.create_variables("g", {20, 30});

    x.set_name("_x");
    p(0).set_name("_p_0");
    p(9).set_name("_p_9");
    g(0, 0).set_name("_g_0_0");
    g(19, 29).set_name("_g_19_29");
    model.setup_unique_name();

    EXPECT_EQ("_x", x.name());
    EXPECT_EQ("_p_0", p(0).name());
    EXPECT_EQ("p[ 1]", p(1).name());
    EXPECT_EQ("p[ 8]", p(8).name());
    EXPECT_EQ("_p_9", p(9).name());
    EXPECT_EQ("_g_0_0", g(0, 0).name());
    EXPECT_EQ("g[ 0,  1]", g(0, 1).name());
    EXPECT_EQ("g[19, 28]", g(19, 28).name());
    EXPECT_EQ("_g_19_29", g(19, 29).name());
}

/*****************************************************************************/
TEST_F(TestModel, setup_is_linear) {
    /// Constraint: linear
    /// Objective: linear
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        model.create_constraint("g", x <= 0);
        model.minimize(x);

        model.setup_is_linear();

        EXPECT_EQ(true, model.is_linear());
    }

    /// Constraint: nonlinear (user-defined lambda)
    /// Objective: linear
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const cppmh::model::Move<int, double>&)> g =
            [&x](const cppmh::model::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", g <= 0);
        model.minimize(x);

        model.setup_is_linear();

        EXPECT_EQ(false, model.is_linear());
    }

    /// Constraint: linear
    /// Objective: nonlinear (user-defined lambda)
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const cppmh::model::Move<int, double>&)> f =
            [&x](const cppmh::model::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", x <= 0);
        model.minimize(f);

        model.setup_is_linear();

        EXPECT_EQ(false, model.is_linear());
    }

    /// Constraint: nonlinear
    /// Objective: nonlinear
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const cppmh::model::Move<int, double>&)> g =
            [&x](const cppmh::model::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        std::function<double(const cppmh::model::Move<int, double>&)> f =
            [&x](const cppmh::model::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", g <= 0);
        model.minimize(f);

        model.setup_is_linear();

        EXPECT_EQ(false, model.is_linear());
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_is_enabled_fast_evaluation) {
    /// Constraint: linear
    /// Objective: linear
    /// User-defined neighborhood: None
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        model.create_constraint("g", x <= 0);
        model.minimize(x);

        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(true, model.is_enabled_fast_evaluation());
    }

    /// Constraint: nonlinear (user-defined lambda)
    /// Objective: linear
    /// User-defined neighborhood: None
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const cppmh::model::Move<int, double>&)> g =
            [&x](const cppmh::model::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", g <= 0);
        model.minimize(x);

        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(false, model.is_enabled_fast_evaluation());
    }

    /// Constraint: linear
    /// Objective: nonlinear (user-defined lambda)
    /// User-defined neighborhood: None
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const cppmh::model::Move<int, double>&)> f =
            [&x](const cppmh::model::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", x <= 0);
        model.minimize(f);

        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(true, model.is_enabled_fast_evaluation());
    }

    /// Constraint: linear
    /// Objective: linear
    /// User-defined neighborhood: Yes
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        model.create_constraint("g", x <= 0);
        model.minimize(x);

        auto move_updater =
            [&x]([[maybe_unused]] std::vector<cppmh::model::Move<int, double>>*
                     a_moves) { ; };

        model.neighborhood().set_user_defined_move_updater(move_updater);
        model.neighborhood().enable_user_defined_move();
        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(false, model.is_enabled_fast_evaluation());
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_sensitivity) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);
    auto& g = model.create_constraints("g", 3);

    auto& p = model.create_expressions("p", 3);
    for (auto i = 0; i < 10; i++) {
        p(0) += (i + 1) * x(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            p(1) += (i + j + 1) * y(i, j);
        }
    }
    g(0) = p(0) <= 10000;
    g(1) = p(1) <= 10000;

    model.minimize(2 * x.sum() + 5 * y.sum());

    model.setup_variable_sensitivity();

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(i + 1, x(i).constraint_sensitivities().at(&g(0)));
        EXPECT_EQ(2, x(i).objective_sensitivity());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(i + j + 1, y(i, j).constraint_sensitivities().at(&g(1)));
            EXPECT_EQ(5, y(i, j).objective_sensitivity());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, presolve) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", 2 * x(0) == 4);
    model.create_constraint("g_1", 3 * x(1) <= 10);
    model.create_constraint("g_2", 8 * x(1) >= 20);
    model.create_constraint("g_3", x(1) + x(2) + 1 == 8);
    model.setup_variable_related_constraints();
    model.setup_is_linear();

    model.presolve(false);

    model.categorize_variables();
    model.categorize_constraints();

    EXPECT_EQ(10, model.number_of_fixed_variables());
    EXPECT_EQ(4, model.number_of_disabled_constraints());
    EXPECT_EQ(true, x(0).is_fixed());
    EXPECT_EQ(2, x(0).value());
    EXPECT_EQ(true, x(1).is_fixed());
    EXPECT_EQ(3, x(1).value());
    EXPECT_EQ(true, x(2).is_fixed());
    EXPECT_EQ(4, x(2).value());

    for (auto i = 3; i < 10; i++) {
        EXPECT_EQ(true, x(i).is_fixed());
        EXPECT_EQ(-10, x(i).value());
    }
}

/*****************************************************************************/
TEST_F(TestModel, remove_independent_variables) {
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        model.remove_independent_variables(false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        model.remove_independent_variables(false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(-x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        model.remove_independent_variables(false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(-x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        model.remove_independent_variables(false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
}

/*****************************************************************************/
TEST_F(TestModel,
       remove_redundant_constraints_with_tightening_variable_bounds) {
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y == 7);
        y.fix_by(1);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);
        y.fix_by(1);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);
        y.fix_by(1);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y == 7);
        y.fix_by(1);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);
        y.fix_by(1);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);
        y.fix_by(1);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);
        x.fix_by(2);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);
        x.fix_by(1);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);
        x.fix_by(3);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);
        x.fix_by(-2);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);
        x.fix_by(-2);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);
        x.fix_by(-2);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);

        model.remove_redundant_constraints_with_tightening_variable_bounds(
            false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestModel, fix_implicit_fixed_variables) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    x(0).set_bound(5, 5);
    model.fix_implicit_fixed_variables(false);
    EXPECT_EQ(5, x(0).value());
    EXPECT_EQ(true, x(0).is_fixed());

    for (auto i = 1; i < 10; i++) {
        EXPECT_EQ(false, x(i).is_fixed());
    }
    model.categorize_variables();
    model.categorize_constraints();

    EXPECT_EQ(1, model.number_of_fixed_variables());
}

/*****************************************************************************/
TEST_F(TestModel, categorize_variables) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    auto& z = model.create_variables("z", {20, 30}, -10, 10);

    x.fix_by(0);
    y(0).fix_by(0);
    z(0, 0).fix_by(0);

    model.categorize_variables();

    EXPECT_EQ(1 + 10 + 20 * 30, model.number_of_variables());
    EXPECT_EQ(3, model.number_of_fixed_variables());
    EXPECT_EQ(11, model.number_of_binary_variables());
    EXPECT_EQ(600, model.number_of_integer_variables());
}

/*****************************************************************************/
TEST_F(TestModel, categorize_constraints) {
    cppmh::model::Model<int, double> model;
    auto coefficients = cppmh::utility::sequence(10);

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", -10, 10);
    auto& z = model.create_variables("z", 10, 0, 1);
    auto& w = model.create_variable("w", 0, 1);
    auto& r = model.create_variables("r", 10, -10, 10);

    auto& singleton = model.create_constraint("singleton");
    singleton       = 2 * x <= 10;

    auto& aggregation = model.create_constraint("aggregation");
    aggregation       = 2 * x + 3 * y == 10;

    auto& precedence = model.create_constraints("precedence", 4);
    precedence(0)    = 2 * x - 2 * y <= 5;
    precedence(1)    = -2 * x + 2 * y <= 5;
    precedence(2)    = 2 * x - 2 * y >= 5;
    precedence(3)    = -2 * x + 2 * y >= 5;

    auto& variable_bound = model.create_constraints("variable_bound", 2);
    variable_bound(0)    = 2 * z(0) + 3 * z(1) >= 5;
    variable_bound(1)    = 2 * z(0) + 3 * z(1) <= 5;

    auto& set_partitioning = model.create_constraint("set_partitioning");
    set_partitioning       = z.selection();

    auto& set_packing = model.create_constraint("set_packing");
    set_packing       = z.sum() <= 1;

    auto& set_covering = model.create_constraint("set_covering");
    set_covering       = z.sum() >= 1;

    auto& cardinality = model.create_constraint("cardinality");
    cardinality       = z.sum() == 5;

    auto& invariant_knapsack = model.create_constraint("invariant_knapsack");
    invariant_knapsack       = z.sum() <= 5;

    auto& equation_knapsack = model.create_constraint("equation_knapsack");
    equation_knapsack       = z.dot(coefficients) == 30;

    auto& bin_packing = model.create_constraints("bin_packing", 2);
    bin_packing(0)    = z.dot(coefficients) + 5 * w <= 5;
    bin_packing(1)    = z.dot(coefficients) - 5 * w >= -5;

    auto& knapsack = model.create_constraints("knapsack", 2);
    knapsack(0)    = z.dot(coefficients) <= 50;
    knapsack(1)    = z.dot(coefficients) >= -50;

    auto& integer_knapsack = model.create_constraints("integer_knapsack", 2);
    integer_knapsack(0)    = r.dot(coefficients) <= 50;
    integer_knapsack(1)    = r.dot(coefficients) >= -50;

    auto& general_liner = model.create_constraint("general_liner");
    general_liner       = x + r.sum() == 50;

    auto& nonlinear = model.create_constraint("nonlinear");
    std::function<double(const cppmh::model::Move<int, double>&)> f =
        [&x](const cppmh::model::Move<int, double>& a_MOVE) {
            return x.evaluate(a_MOVE) - 1;
        };
    nonlinear = f <= 5;

    singleton.disable();

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(cppmh::model::SelectionMode::Defined);

    EXPECT_EQ(22, model.number_of_constraints());
    EXPECT_EQ(1, model.number_of_selection_constraints());
    EXPECT_EQ(2, model.number_of_disabled_constraints());

    auto reference = model.constraint_type_reference();
    EXPECT_EQ(1, static_cast<int>(reference.singleton_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.aggregation_ptrs.size()));
    EXPECT_EQ(4, static_cast<int>(reference.precedence_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.variable_bound_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.set_partitioning_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.set_packing_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.set_covering_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.invariant_knapsack_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.equation_knapsack_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.bin_packing_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.knapsack_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.integer_knapsack_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.general_linear_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.nonlinear_ptrs.size()));
}

/*****************************************************************************/
TEST_F(TestModel, extract_selections_larger) {
    cppmh::model::Model<int, double> model;

    auto& x0 = model.create_variables("x0", {10, 10}, 0, 1);
    auto& x1 = model.create_variables("x1", {20, 20}, 0, 1);
    auto& x2 = model.create_variables("x2", 2, 0, 1);

    auto& y = model.create_variables("y", {30, 30}, -10, 10);

    /**
     * Selection constraint with 10 decision variables. The priority of this
     * constraint is the third, and it will be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c0", x0.selection({0, cppmh::model::Range::All}));

    /**
     * Selection constraint with 32 decision variables. The priority of this
     * constraint is the second, and it will NOT be employed for a swap
     * neighborhood because higher-priority constraint c1 covers x1.
     */
    model.create_constraint(
        "c1", (x0.sum({1, cppmh::model::Range::All}) +
               x1.sum({1, cppmh::model::Range::All}) + x2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables. The priority of this
     * constraint is the first, and it will be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c2", x1.selection());

    /**
     * Selection constraint with 2 decision variables. The priority of this
     * constraint is the fourth, and it will NOT be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c3", x2.selection());

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(cppmh::model::SelectionMode::Larger);

    EXPECT_EQ(2, model.number_of_selection_constraints());
    EXPECT_EQ(2, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c2
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(400, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(19, 19)) != variable_ptrs.end()));
    }

    {
        /// Constraint c0
        auto variable_ptrs = model.selections()[1].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 0)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */
    /// Constraint c2
    EXPECT_EQ(false, model.selections()[0].constraint_ptr->is_enabled());

    /// Constraint c0
    EXPECT_EQ(false, model.selections()[1].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category. */

    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(20 * 20 + 1 * 10, model.number_of_selection_variables());

        /// Constraint c2
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(19, 19)) != variable_ptrs.end()));

        /// Constraint c0
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 0)) != variable_ptrs.end()));
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - (20 * 20 + 1 * 10),
                  model.number_of_binary_variables());

        /// Constraint c2
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(19, 19)) != variable_ptrs.end()));

        /// Constraint c0
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(1, 0)) != variable_ptrs.end()));
    }

    /// Integer
    {
        auto variable_ptrs = model.variable_reference().integer_variable_ptrs;
        EXPECT_EQ(30 * 30, model.number_of_integer_variables());

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(29, 29)) != variable_ptrs.end()));
    }
}

/*****************************************************************************/
TEST_F(TestModel, extract_selections_independent) {
    cppmh::model::Model<int, double> model;

    auto& x0 = model.create_variables("x0", {10, 10}, 0, 1);
    auto& x1 = model.create_variables("x1", {20, 20}, 0, 1);
    auto& x2 = model.create_variables("x2", 2, 0, 1);

    auto& y = model.create_variables("y", {30, 30}, -10, 10);

    /**
     * Selection constraint with 10 decision variables (no overlap).
     */
    model.create_constraint("c0", x0.selection({0, cppmh::model::Range::All}));

    /**
     * Selection constraint with 32 decision variables (overlap).
     */
    model.create_constraint(
        "c1", (x0.sum({1, cppmh::model::Range::All}) +
               x1.sum({1, cppmh::model::Range::All}) + x2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables (overlap).
     */
    model.create_constraint("c2", x1.selection());

    /**
     * Selection constraint with 2 decision variables (overlap).
     */
    model.create_constraint("c3", x2.selection());

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(cppmh::model::SelectionMode::Independent);

    EXPECT_EQ(1, model.number_of_selection_constraints());
    EXPECT_EQ(1, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c0
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 0)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */
    /// Constraint c0
    EXPECT_EQ(false, model.selections()[0].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category. */
    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(10, model.number_of_selection_variables());

        /// Constraint c0
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 0)) != variable_ptrs.end()));
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - 10,
                  model.number_of_binary_variables());

        /// Constraint c0
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(1, 0)) != variable_ptrs.end()));
    }

    /// Integer
    {
        auto variable_ptrs = model.variable_reference().integer_variable_ptrs;
        EXPECT_EQ(30 * 30, model.number_of_integer_variables());

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(29, 29)) != variable_ptrs.end()));
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_neighborhood) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
TEST_F(TestModel, verify_and_correct_selection_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(2);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(true,
                                                                        false),
            std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(2);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There is one fixed selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);
        EXPECT_EQ(1, x(0).value());
    }

    /// There is one fixed selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(false,
                                                                    false);
        EXPECT_EQ(1, x(0).value());
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(true,
                                                                        false),
            std::logic_error);
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There are two variables with invalid initial values.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 2;
        x(1) = 3;
        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        EXPECT_EQ(0, x(0).value());
        EXPECT_EQ(0, x(1).value());
    }

    /// There are two variables with invalid initial values.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 2;
        x(1) = 3;

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There is no selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        /// selected_variable_ptr is not always &x(0).
        auto selected_variable_ptr =
            model.selections().front().variable_ptrs.front();
        EXPECT_EQ(1, selected_variable_ptr->value());
    }

    /// There is no selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There is one selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        EXPECT_EQ(1, x(0).value());
    }

    /// There is one selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(false,
                                                                    false);

        EXPECT_EQ(1, x(0).value());
    }

    /// There are two unfixed selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        /// selected_variable is not always x(0).
        EXPECT_EQ(1, x(0).value() + x(1).value());
    }

    /// There are two unfixed selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);
        EXPECT_EQ(0, x(0).value());
        EXPECT_EQ(1, x(1).value());
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, verify_and_correct_binary_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0).fix_by(2);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(model.verify_and_correct_binary_variables_initial_values(
                         true, false),
                     std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0).fix_by(-1);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(model.verify_and_correct_binary_variables_initial_values(
                         false, false),
                     std::logic_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0)    = 2;
        x(1)    = -1;

        model.categorize_variables();
        model.categorize_constraints();

        model.verify_and_correct_binary_variables_initial_values(true, false);
        EXPECT_EQ(1, x(0).value());
        EXPECT_EQ(0, x(1).value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0)    = 2;
        x(1)    = -1;

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(model.verify_and_correct_binary_variables_initial_values(
                         false, false),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, verify_and_correct_integer_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0).fix_by(11);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(model.verify_and_correct_integer_variables_initial_values(
                         true, false),
                     std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0).fix_by(-11);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(model.verify_and_correct_integer_variables_initial_values(
                         false, false),
                     std::logic_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0)    = 11;
        x(1)    = -11;

        model.categorize_variables();
        model.categorize_constraints();

        model.verify_and_correct_integer_variables_initial_values(true, false);
        EXPECT_EQ(10, x(0).value());
        EXPECT_EQ(-10, x(1).value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0)    = 11;
        x(1)    = -11;

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(model.verify_and_correct_integer_variables_initial_values(
                         false, false),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_fixed_sensitivities) {
    /// This method is tested in test_expression.h
}

/*****************************************************************************/
TEST_F(TestModel, set_callback) {
    cppmh::model::Model<int, double> model;

    int  v = 1;
    auto f = [&v](void) { v++; };
    model.set_callback(f);
    model.callback();
    EXPECT_EQ(2, v);
}

/*****************************************************************************/
TEST_F(TestModel, callback) {
    /// This method is tested in set_callback.
}

/*****************************************************************************/
TEST_F(TestModel, import_variable_values) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    cppmh::model::ValueProxy<int> x_value(x.id());
    cppmh::model::ValueProxy<int> y_value(y.id(), 10);
    cppmh::model::ValueProxy<int> z_value(z.id(), {10, 10});

    x_value.value() = 1;

    for (auto i = 0; i < 10; i++) {
        y_value(i) = 10 * i;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z_value(i, j) = 100 * (i + j);
        }
    }

    model.import_variable_values({x_value, y_value, z_value});

    EXPECT_EQ(1, x.value());

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(10 * i, y(i).value());
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            EXPECT_EQ(100 * (i + j), z(i, j).value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, update_arg_void) {
    cppmh::model::Model<int, double> model;

    auto sequence = cppmh::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    model.create_constraint("g", x.selection());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.minimize(p);
    model.update();

    // 0 + 1 + 2 + ... + 9 + 1 = 46
    EXPECT_EQ(46, p.value());
    EXPECT_EQ(46, p(0).value());
    EXPECT_EQ(46, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, update_arg_move) {
    cppmh::model::Model<int, double> model;

    auto sequence = cppmh::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    model.create_constraint("g", x.selection());

    x(0) = 1;

    model.minimize(p);
    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(cppmh::model::SelectionMode::Defined);
    model.setup_fixed_sensitivities(false);

    model.update();

    cppmh::model::Move<int, double> move;
    move.sense = cppmh::model::MoveSense::Selection;
    move.alterations.emplace_back(&x(0), 0);
    move.alterations.emplace_back(&x(9), 1);

    model.update(move);

    EXPECT_EQ(10, p.value());
    EXPECT_EQ(10, p(0).value());
    EXPECT_EQ(10, model.objective().value());
    EXPECT_EQ(&x(9), model.selections().front().selected_variable_ptr);
}

/*****************************************************************************/
TEST_F(TestModel, evaluate) {
    /// minimize
    {
        cppmh::model::Model<int, double> model;

        auto sequence = cppmh::utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        cppmh::model::ValueProxy<double> local_penalty_coefficient_proxy(1);
        local_penalty_coefficient_proxy.value() = 100;

        cppmh::model::ValueProxy<double> global_penalty_coefficient_proxy(1);
        global_penalty_coefficient_proxy.value() = 10000;

        std::vector<cppmh::model::ValueProxy<double>>
            local_penalty_coefficient_proxies = {
                local_penalty_coefficient_proxy,
                local_penalty_coefficient_proxy};
        std::vector<cppmh::model::ValueProxy<double>>
            global_penalty_coefficient_proxies = {
                global_penalty_coefficient_proxy,
                global_penalty_coefficient_proxy};

        model.minimize(p);

        model.setup_variable_related_constraints();
        model.setup_variable_sense();
        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);
        model.setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }

        model.update();
        auto score_before =
            model.evaluate({}, local_penalty_coefficient_proxies,
                           global_penalty_coefficient_proxies);

        {
            cppmh::model::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 =
                model.evaluate(move, local_penalty_coefficient_proxies,
                               global_penalty_coefficient_proxies);

            auto score_after_1 = model.evaluate(
                move, score_before, local_penalty_coefficient_proxies,
                global_penalty_coefficient_proxies);

            EXPECT_EQ(46, score_after_0.objective);
            EXPECT_EQ(5 + 1, score_after_0.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_0.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_0.global_penalty);
            EXPECT_EQ(46 + 5 * 100 + 100,
                      score_after_0.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000 + 10000,
                      score_after_0.global_augmented_objective);
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_constraint_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_constraint_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            cppmh::model::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 =
                model.evaluate(move, local_penalty_coefficient_proxies,
                               global_penalty_coefficient_proxies);

            auto score_after_1 = model.evaluate(
                move, score_before, local_penalty_coefficient_proxies,
                global_penalty_coefficient_proxies);

            EXPECT_EQ(1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(1, score_after_0.local_augmented_objective);
            EXPECT_EQ(1, score_after_0.global_augmented_objective);
            EXPECT_EQ(true, score_after_0.is_objective_improvable);
            EXPECT_EQ(true, score_after_0.is_constraint_improvable);
            EXPECT_EQ(true, score_after_0.is_feasible);

            EXPECT_EQ(1, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(1, score_after_1.local_augmented_objective);
            EXPECT_EQ(1, score_after_1.global_augmented_objective);
            EXPECT_EQ(true, score_after_1.is_objective_improvable);
            EXPECT_EQ(true, score_after_1.is_constraint_improvable);
            EXPECT_EQ(true, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            cppmh::model::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 =
                model.evaluate(move, local_penalty_coefficient_proxies,
                               global_penalty_coefficient_proxies);

            auto score_after_1 = model.evaluate(
                move, score_before, local_penalty_coefficient_proxies,
                global_penalty_coefficient_proxies);

            EXPECT_EQ(11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_constraint_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_constraint_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
    }

    /// maximize
    {
        cppmh::model::Model<int, double> model;

        auto sequence = cppmh::utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        cppmh::model::ValueProxy<double> local_penalty_coefficient_proxy(1);
        local_penalty_coefficient_proxy.value() = 100;

        cppmh::model::ValueProxy<double> global_penalty_coefficient_proxy(1);
        global_penalty_coefficient_proxy.value() = 10000;

        std::vector<cppmh::model::ValueProxy<double>>
            local_penalty_coefficient_proxies = {
                local_penalty_coefficient_proxy,
                local_penalty_coefficient_proxy};
        std::vector<cppmh::model::ValueProxy<double>>
            global_penalty_coefficient_proxies = {
                global_penalty_coefficient_proxy,
                global_penalty_coefficient_proxy};

        model.maximize(p);

        model.setup_variable_related_constraints();
        model.setup_variable_sense();
        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(cppmh::model::SelectionMode::Defined);
        model.setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }
        model.update();
        auto score_before =
            model.evaluate({}, local_penalty_coefficient_proxies,
                           global_penalty_coefficient_proxies);

        {
            cppmh::model::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 =
                model.evaluate(move, local_penalty_coefficient_proxies,
                               global_penalty_coefficient_proxies);
            auto score_after_1 = model.evaluate(
                move, score_before, local_penalty_coefficient_proxies,
                global_penalty_coefficient_proxies);

            EXPECT_EQ(-46, score_after_0.objective);
            EXPECT_EQ(5 + 1, score_after_0.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_0.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_0.global_penalty);
            EXPECT_EQ(-46 + 5 * 100 + 100,
                      score_after_0.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000 + 10000,
                      score_after_0.global_augmented_objective);
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_constraint_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(-46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(-46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_constraint_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
        {
            cppmh::model::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 =
                model.evaluate(move, local_penalty_coefficient_proxies,
                               global_penalty_coefficient_proxies);

            auto score_after_1 = model.evaluate(
                move, score_before, local_penalty_coefficient_proxies,
                global_penalty_coefficient_proxies);

            EXPECT_EQ(-1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(-1, score_after_0.local_augmented_objective);
            EXPECT_EQ(-1, score_after_0.global_augmented_objective);
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(true, score_after_0.is_constraint_improvable);
            EXPECT_EQ(true, score_after_0.is_feasible);

            EXPECT_EQ(-1, score_after_1.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(-1, score_after_1.local_augmented_objective);
            EXPECT_EQ(-1, score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(true, score_after_1.is_constraint_improvable);
            EXPECT_EQ(true, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            cppmh::model::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 =
                model.evaluate(move, local_penalty_coefficient_proxies,
                               global_penalty_coefficient_proxies);

            auto score_after_1 = model.evaluate(
                move, score_before, local_penalty_coefficient_proxies,
                global_penalty_coefficient_proxies);

            EXPECT_EQ(-11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_constraint_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(-11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_constraint_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, compute_lagrangian) {
    cppmh::model::Model<int, double> model;

    auto sequence = cppmh::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
    [[maybe_unused]] auto& h = model.create_constraint("h", x(0) + x(1) <= 1);

    model.minimize(p);
    model.categorize_variables();
    model.categorize_constraints();

    cppmh::model::ValueProxy<double> dual_value_proxy(1);
    dual_value_proxy.value() = 100;

    std::vector<cppmh::model::ValueProxy<double>> dual_value_proxies = {
        dual_value_proxy, dual_value_proxy};

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }

    model.update();
    auto lagrangian = model.compute_lagrangian(dual_value_proxies);

    EXPECT_EQ(46 + 100 * (10 - 5) + 100 * (2 - 1), lagrangian);
}

/*****************************************************************************/
TEST_F(TestModel, generate_variable_parameter_proxies) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_name();

    auto parameter_proxies =
        model.generate_variable_parameter_proxies(fill_value);
    EXPECT_EQ(x.id(), parameter_proxies[0].id());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("x", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(y.id(), parameter_proxies[1].id());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("y[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("y[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(z.id(), parameter_proxies[2].id());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("z[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("z[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, generate_expression_parameter_proxies) {
    cppmh::model::Model<int, double> model;

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_name();

    auto parameter_proxies =
        model.generate_expression_parameter_proxies(fill_value);
    EXPECT_EQ(p.id(), parameter_proxies[0].id());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("p", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(q.id(), parameter_proxies[1].id());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("q[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("q[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(r.id(), parameter_proxies[2].id());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("r[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("r[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, generate_constraint_parameter_proxies) {
    cppmh::model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_name();

    auto parameter_proxies =
        model.generate_constraint_parameter_proxies(fill_value);
    EXPECT_EQ(g.id(), parameter_proxies[0].id());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("g", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(h.id(), parameter_proxies[1].id());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("h[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("h[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(v.id(), parameter_proxies[2].id());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("v[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("v[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, export_solution) {
    cppmh::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {20, 30});

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {20, 30});

    p = random_integer() * x;
    for (auto i = 0; i < 10; i++) {
        q(i) = random_integer() * y(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            r(i, j) = random_integer() * z(i, j) + random_integer();
            v(i, j) = r(i, j) == random_integer();
        }
    }
    model.minimize(random_integer() * p + random_integer() * q.sum() +
                   random_integer() * r.sum());

    x = random_integer();
    for (auto i = 0; i < 10; i++) {
        y(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            z(i, j) = random_integer();
        }
    }

    model.update();

    /// Test Solution
    auto solution = model.export_solution();
    EXPECT_EQ(3, static_cast<int>(solution.variable_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.expression_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.constraint_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.violation_value_proxies.size()));

    EXPECT_EQ(x.id(), solution.variable_value_proxies[0].id());
    EXPECT_EQ(x.value(), solution.variable_value_proxies[0].value());

    EXPECT_EQ(y.id(), solution.variable_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), solution.variable_value_proxies[1](i));
    }

    EXPECT_EQ(z.id(), solution.variable_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(),
                      solution.variable_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(p.id(), solution.expression_value_proxies[0].id());
    EXPECT_EQ(p.value(), solution.expression_value_proxies[0].value());

    EXPECT_EQ(q.id(), solution.expression_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(q(i).value(), solution.expression_value_proxies[1](i));
    }

    EXPECT_EQ(r.id(), solution.expression_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(r(i, j).value(),
                      solution.expression_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(g.id(), solution.constraint_value_proxies[0].id());
    EXPECT_EQ(g.constraint_value(),
              solution.constraint_value_proxies[0].value());

    EXPECT_EQ(h.id(), solution.constraint_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).constraint_value(),
                  solution.constraint_value_proxies[1](i));
    }

    EXPECT_EQ(v.id(), solution.constraint_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).constraint_value(),
                      solution.constraint_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(g.id(), solution.violation_value_proxies[0].id());
    EXPECT_EQ(g.violation_value(), solution.violation_value_proxies[0].value());

    EXPECT_EQ(h.id(), solution.constraint_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).violation_value(),
                  solution.violation_value_proxies[1](i));
    }

    EXPECT_EQ(v.id(), solution.violation_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).violation_value(),
                      solution.violation_value_proxies[2](i, j));
        }
    }

    /// Test NamedSolution
    auto named_solution = model.convert_to_named_solution(solution);
    EXPECT_EQ(3, static_cast<int>(named_solution.variables().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.expressions().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.constraints().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.violations().size()));

    EXPECT_EQ(x.id(), named_solution.variables("x").id());
    EXPECT_EQ(x.id(), named_solution.variables().at("x").id());
    EXPECT_EQ(x.value(), named_solution.variables("x").value());
    EXPECT_EQ(x.value(), named_solution.variables().at("x").value());

    EXPECT_EQ(y.id(), named_solution.variables("y").id());
    EXPECT_EQ(y.id(), named_solution.variables().at("y").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), named_solution.variables("y")(i));
        EXPECT_EQ(y(i).value(), named_solution.variables().at("y")(i));
    }

    EXPECT_EQ(z.id(), named_solution.variables("z").id());
    EXPECT_EQ(z.id(), named_solution.variables().at("z").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(), named_solution.variables("z")(i, j));
            EXPECT_EQ(z(i, j).value(),
                      named_solution.variables().at("z")(i, j));
        }
    }

    EXPECT_EQ(p.id(), named_solution.expressions("p").id());
    EXPECT_EQ(p.id(), named_solution.expressions().at("p").id());
    EXPECT_EQ(p.value(), named_solution.expressions("p").value());
    EXPECT_EQ(p.value(), named_solution.expressions().at("p").value());

    EXPECT_EQ(q.id(), named_solution.expressions("q").id());
    EXPECT_EQ(q.id(), named_solution.expressions().at("q").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(q(i).value(), named_solution.expressions("q")(i));
        EXPECT_EQ(q(i).value(), named_solution.expressions().at("q")(i));
    }

    EXPECT_EQ(r.id(), named_solution.expressions("r").id());
    EXPECT_EQ(r.id(), named_solution.expressions().at("r").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(r(i, j).value(), named_solution.expressions("r")(i, j));
            EXPECT_EQ(r(i, j).value(),
                      named_solution.expressions().at("r")(i, j));
        }
    }

    EXPECT_EQ(g.id(), named_solution.constraints("g").id());
    EXPECT_EQ(g.id(), named_solution.constraints().at("g").id());
    EXPECT_EQ(g.constraint_value(), named_solution.constraints("g").value());
    EXPECT_EQ(g.constraint_value(),
              named_solution.constraints().at("g").value());

    EXPECT_EQ(h.id(), named_solution.constraints("h").id());
    EXPECT_EQ(h.id(), named_solution.constraints().at("h").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).constraint_value(), named_solution.constraints("h")(i));
        EXPECT_EQ(h(i).constraint_value(),
                  named_solution.constraints().at("h")(i));
    }

    EXPECT_EQ(v.id(), named_solution.constraints("v").id());
    EXPECT_EQ(v.id(), named_solution.constraints().at("v").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).constraint_value(),
                      named_solution.constraints("v")(i, j));
            EXPECT_EQ(v(i, j).constraint_value(),
                      named_solution.constraints().at("v")(i, j));
        }
    }

    EXPECT_EQ(g.id(), named_solution.violations("g").id());
    EXPECT_EQ(g.id(), named_solution.violations().at("g").id());
    EXPECT_EQ(g.violation_value(), named_solution.violations("g").value());
    EXPECT_EQ(g.violation_value(), named_solution.violations().at("g").value());

    EXPECT_EQ(h.id(), named_solution.violations("h").id());
    EXPECT_EQ(h.id(), named_solution.violations().at("h").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).violation_value(), named_solution.violations("h")(i));
        EXPECT_EQ(h(i).violation_value(),
                  named_solution.violations().at("h")(i));
    }

    EXPECT_EQ(v.id(), named_solution.violations("v").id());
    EXPECT_EQ(v.id(), named_solution.violations().at("v").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).violation_value(),
                      named_solution.violations("v")(i, j));
            EXPECT_EQ(v(i, j).violation_value(),
                      named_solution.violations().at("v")(i, j));
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, convert_to_named_solution) {
    /// This method is tested in export_solution.
}

/*****************************************************************************/
TEST_F(TestModel, variable_proxies) {
    /// This method is tested in create_variable_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, expression_proxies) {
    /// This method is tested in create_expression_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_proxies) {
    /// This method is tested in create_constraint_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, objective) {
    /// This method is tested in minimize_arg_function and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_names) {
    /// This method is tested in create_variable_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, expression_names) {
    /// This method is tested in create_expression_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_names) {
    /// This method is tested in create_constraint_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, selections) {
    /// This method is tested in extract_selections_larger() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_reference) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_reference) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_type_reference) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, is_defined_objective) {
    /// This method is tested in minimize_arg_function() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, is_enabled_fast_evaluation) {
    /// This method is tested in setup_is_enabled_fast_evaluation().
}

/*****************************************************************************/
TEST_F(TestModel, is_linear) {
    /// This method is tested in setup_is_linear().
}

/*****************************************************************************/
TEST_F(TestModel, is_minimization) {
    /// This method is tested in minimize_arg_function() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, sign) {
    /// This method is tested in minimize_arg_function() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, number_of_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_fixed_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_selection_variables) {
    /// This method is tested in extract_selections_larger() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, number_of_binary_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_integer_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_constraints) {
    /// This method is tested in categorize_constraints().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_selection_constraints) {
    /// This method is tested in extract_selections_larger() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, number_of_disabled_constraints) {
    /// This method is tested in categorize_constraints().
}

/*****************************************************************************/
TEST_F(TestModel, neighborhood) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/